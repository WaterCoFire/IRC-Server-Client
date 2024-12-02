/*
CONTRIBUTIONS OF THIS CODE FILE
Everyone in the group contributed to this code file.
Specific contributors of each function is provided in comments at the head of it.
*/

#include <iostream>
#include <algorithm>
#include <chrono>
#include <string>
#include <ctime>
#include <iomanip>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <windows.h>
#include <vector>
#include <algorithm> // std::find
#include <fstream>
#include <sstream> // for stringstream

#pragma comment(lib, "ws2_32.lib")

void handleServerResponse(SOCKET sock, const std::string &botName, const std::string &channel);
void sendCommand(SOCKET sock, const std::string &command);
DWORD WINAPI inputThread(LPVOID param);
std::vector<std::string> currentUsers;
boolean isExit = false;

/*
Converts an IP address from text to binary format
*/
// Contributors: Yao Yao
struct ThreadParams
{
    SOCKET sock;
    std::string channel;
    std::string user;
};

/*
Converts an IP address from text to binary format
*/
// Contributors: Siqi Peng, Jieshen Cai
int inet(int af, const char *src, void *dst)
{
    struct sockaddr_storage ss;
    int size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN + 1];

    ZeroMemory(&ss, sizeof(ss));
    strncpy(src_copy, src, INET6_ADDRSTRLEN + 1);
    src_copy[INET6_ADDRSTRLEN] = 0;

    // Convert the string IP address to binary format
    if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0)
    {
        switch (af)
        {
        case AF_INET:
            *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
            return 1;
        case AF_INET6:
            *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
            return 1;
        }
    }
    return 0;
}

/*
Select statements from funfact.txt for random replies
*/
// Contributors: Yao Yao, Jiangtao Ma
std::string getRandomFunFact()
{
    std::ifstream file("funfacts.txt");
    std::vector<std::string> facts;
    std::string line;

    // Check status
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open fun_facts.txt" << std::endl;
        return "No fun facts available.";
    }

    // Read each line of the file and store it in a vector
    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            facts.push_back(line);
        }
    }

    file.close();

    // If no sentences are read, return a warning
    if (facts.empty())
    {
        return "No fun facts available.";
    }

    return facts[rand() % facts.size()];
}

/*
Get a random user
Users are randomly selected and senders and bots are excluded
*/
// Contributors: Yao Yao, Shunyao Wang
std::string getRandomUser(const std::vector<std::string> &users, const std::string &botName, const std::string &sender)
{
    std::vector<std::string> validUsers;
    for (const std::string &user : users)
    {
        if (user != botName && user != sender)
        {
            validUsers.push_back(user);
        }
    }

    if (validUsers.empty())
    {
        return sender; // If there are no other users, the sender is selected
    }

    return validUsers[rand() % validUsers.size()]; // Select a user at random
}

// Send NAMES to user
// Contributors: Jiangtao Ma
void requestChannelUsers(SOCKET sock, const std::string &channel)
{
    sendCommand(sock, "NAMES " + channel + "\r\n");
}

// Send commands to the server
// Contributors: Jieshen Cai
void sendCommand(SOCKET sock, const std::string &command)
{
    if (command.empty())
    {
        std::cerr << "Attempted to send an empty command" << std::endl;
        return;
    }
    int bytesSent = send(sock, command.c_str(), command.length(), 0);
    if (bytesSent == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }
}

/*
Handling the server response
*/
// Contributors: Siqi Peng, Shunyao Wang, Jieshen Cai, Yao Yao, Jiangtao Ma
void handleServerResponse(SOCKET sock, const std::string &botName, const std::string &channel)
{
    char buffer[1024];
    std::string response;

    while (!isExit)
    {
        memset(buffer, 0, 1024);                             // Clear buffer
        int bytesReceived = recv(sock, buffer, 1024 - 1, 0); // Receive data
        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "Socket error occurred" << std::endl;
            break;
        }
        else if (bytesReceived == 0)
        {
            std::cerr << "Connection closed" << std::endl;
            break;
        }

        buffer[bytesReceived] = '\0';
        response = std::string(buffer);

        // Handling Ping commands
        if (response.find("PING :") == 0)
        {
            std::string pongResponse = "PONG " + response.substr(5) + "\r\n";
            sendCommand(sock, pongResponse);

            continue;
        }

        if (response.find("redundant nickname error") != std::string::npos)
        {
            std::cout << "redundant nickname error";
            closesocket(sock);
            WSACleanup();
            exit(0);
        }

        if (response.find(" 311 ") != std::string::npos)
        { // WHOIS user info response
            std::cout << "WHOIS Info: " << response << std::endl;
        }

        /// Handle PRIVMSG messages
        size_t privmsgPos = response.find(" PRIVMSG ");
        if (privmsgPos != std::string::npos)
        {
            // Extract the message sender
            size_t senderEnd = response.find('!');
            std::string sender = response.substr(1, senderEnd - 1); // Extract the sender's name

            // Extract the receiver
            size_t targetStart = privmsgPos + 9; // 9 = length of " PRIVMSG "
            size_t targetEnd = response.find(" :", targetStart);
            std::string target = response.substr(targetStart, targetEnd - targetStart);

            // Extract message content
            std::string message = response.substr(targetEnd + 2);
            std::string channel1 = "";
            if (response.find("#") != std::string::npos)
            {
                channel1 = response.substr(response.find("#"));
                channel1 = channel1.substr(0, channel1.find("\r\n"));
                channel1 = channel1.substr(0, channel1.find(" :"));
            }

            // If the message is sent to a channel, it is a public message
            if (target == channel1)
            {

                std::cout << sender << ": " << message << std::endl;
            }
            // If the message is sent to a bot, it is a private message
            else if (target == botName)
            {
                std::cout << sender << ": " << message << std::endl;
            }
            // SuperBot should reply with a fun fact
            if (botName == "SuperBot")
            {
                sendCommand(sock, "PRIVMSG " + sender + " :" + getRandomFunFact() + "\r\n");
            }
            continue;
        }
        size_t errPos = response.find("ERROR");
        if (errPos != std::string::npos)
        {
            std::cout << response.substr(errPos);
        }

        // Handling the NAMES response (list of users)
        size_t namesPos = response.find(" 353 "); // 353 is RPL_NAMREPLY
        if (namesPos != std::string::npos)
        {
            // Find the section after the ":", which is the list of usernames
            size_t usersStart = response.find(" :", namesPos);
            size_t usersEnd = response.find("\r\n", usersStart);
            if (usersStart != std::string::npos && usersEnd != std::string::npos)
            {
                // Extract the list of usernames and remove the ":"
                std::string userList = response.substr(usersStart + 2, usersEnd - usersStart - 2);
                currentUsers.clear();
                // Use a stringstream to split usernames by whitespace
                std::stringstream ss(userList);
                std::string user;
                while (ss >> user)
                {
                    currentUsers.push_back(user);
                }
            }
        }

        /*
        Handle all the keywords detected
        */
        // TIME
        size_t timePos = response.find("/time: ");
        if (timePos != std::string::npos)
        {
            std::string channel1 = response.substr(response.find("#"));
            channel1 = channel1.substr(0, channel1.find("\r\n"));
            channel1 = channel1.substr(0, channel1.find("/"));
            std::cout << response.substr(timePos + 1);

            sendCommand(sock, "PRIVMSG " + channel1 + " :" + response.substr(timePos + 7));
        }

        // KICK
        size_t kickPos = response.find("/kick: ");
        if (kickPos != std::string::npos)
        {
            std::string channel1 = response.substr(response.find("#"));
            channel1 = channel1.substr(0, channel1.find("\r\n"));
            channel1 = channel1.substr(0, channel1.find("/"));

            std::cout << response.substr(kickPos + 1);
            sendCommand(sock, "PRIVMSG " + channel1 + " :" + response.substr(kickPos + 7));
        }

        // MODE
        size_t modePos = response.find("/mode: ");
        if (modePos != std::string::npos)
        {
            std::string channel1 = response.substr(response.find("#"));
            channel1 = channel1.substr(0, channel1.find("\r\n"));
            channel1 = channel1.substr(0, channel1.find("!"));
            std::cout << response.substr(modePos + 1);
            sendCommand(sock, "PRIVMSG " + channel1 + " :" + response.substr(modePos + 7));
        }

        /// Handle PRIVMSG messages
        size_t helloPos = response.find("/hello");
        if (helloPos != std::string::npos)
        {
            // Extract the message sender
            size_t senderEnd = response.find('!');
            std::string sender = response.substr(1, senderEnd - 1); // Extract the sender's name

            std::cout << "Hello Pos" + sender << std::endl;

            sendCommand(sock, "PRIVMSG " + sender + " :" + "Hello, " + sender + "!\r\n");
        }

        // size_t timePos = response.find("SuperBot");
        //  if (timePos != std::string::npos) {
        //    std::string res = response.substr(timePos);
        //    std::cout <<res;
        // }

        // TOPIC
        size_t topicPos = response.find("/topic: ");
        if (topicPos != std::string::npos)
        {
            std::cout << response.substr(topicPos + 8);
            std::string channel1 = response.substr(response.find("#"));
            channel1 = channel1.substr(0, channel1.find("\r\n"));
            channel1 = channel1.substr(0, channel1.find("'s"));
            sendCommand(sock, "PRIVMSG " + channel1 + " :" + response.substr(topicPos + 8));
        }

        // WHOIS
        size_t whoamiPos = response.find("/whois: ");
        if (whoamiPos != std::string::npos)
        {
            std::cout << "Bot sends:" + response.substr(whoamiPos + 8);

            std::string channel1 = response.substr(response.find("#"));
            channel1 = channel1.substr(0, channel1.find("\r\n"));
            channel1 = channel1.substr(0, channel1.find("/"));
            sendCommand(sock, "PRIVMSG " + channel1 + " :" + response.substr(whoamiPos + 8));
        }
        // SLAP
        size_t slapPos = response.find(" /slap");
        if (slapPos != std::string::npos)
        {
            // Extract the message sender
            size_t senderEnd = response.find('!');
            std::string sender = response.substr(1, senderEnd - 1); // Extract the sender's name

            // Extract the part after "/slap" (this could be a target user or empty)
            size_t targetStart = slapPos + 7;                   // Position right after " /slap"
            size_t messageEnd = response.find("\r\n", slapPos); // Find end of the message

            std::string target;
            // Check if there is a target (content after "/slap" and before the message end)
            if (targetStart < messageEnd)
            {
                target = response.substr(targetStart, messageEnd - targetStart); // Extract the target user
                target = target.empty() ? "" : target;                           // Check if target is empty
            }
            else
            {
                target = ""; // No target specified
            }
            std::string channel1 = response.substr(response.find("#"));
            channel1 = channel1.substr(0, channel1.find("\r\n"));
            channel1 = channel1.substr(0, channel1.find(" :"));
            // Request the server to send a list of NAMES
            sendCommand(sock, "NAMES " + channel1 + "\r\n");

            std::cout << "Bot sends: Current users in the channel: ";

            for (const auto &user : currentUsers)
            {
                std::cout << user << " ";
            }

            std::cout << "Target: " + target << std::endl;
            // If a user is specified, check whether the user is in the channel
            if (!target.empty())
            {
                if (std::find(currentUsers.begin(), currentUsers.end(), target) != currentUsers.end())
                {
                    sendCommand(sock, "PRIVMSG " + channel1 + " :slaps " + target + " !!!\r\n");
                }
                else
                {
                    sendCommand(sock, "PRIVMSG " + channel1 + " :slaps " + sender + " himself  (User not found)!!!\r\n");
                }
            }
            else
            {
                // Select a user at random
                std::string slappedUser = getRandomUser(currentUsers, sender, "SuperBot"); // Let's say the sender's name is "User".
                sendCommand(sock, "PRIVMSG " + channel1 + " :slaps " + slappedUser + " !!!\r\n");
            }
        }
    }
}

/*
A thread that processes user input
*/
// Contributors: Shunyao Wang, Jiangtao Ma, Yao Yao
DWORD WINAPI inputThread(LPVOID param)
{
    ThreadParams *params = (ThreadParams *)param;
    SOCKET sock = params->sock;
    std::string channel = params->channel;
    std::string myname = params->user;

    while (true)
    {
        std::string userInput;
        std::getline(std::cin, userInput);

        // Check if the message is private
        if (userInput.find("/msg ") == 0)
        {
            std::string privateMessage = userInput.substr(5); // remove "/msg "
            size_t spacePos = privateMessage.find(' ');
            if (spacePos != std::string::npos)
            {
                std::string targetUser = privateMessage.substr(0, spacePos);
                std::string message = privateMessage.substr(spacePos + 1);
                sendCommand(sock, "PRIVMSG " + targetUser + " :" + message + "\r\n");
            }
            else
            {
                std::cerr << "Invalid private message format. Use: /msg <user> <message>" << std::endl;
            }
        }
        // Join channel command
        else if (userInput.find("/join") == 0)
        {
            if (userInput.find("#") == std::string::npos)
            {
                std::cout << "ERROR: you should add '#' before channel\r\n";
                break;
            }
            std::string channel1 = userInput.substr(userInput.find("#"));
            params->channel = channel1;
            channel = channel1;
            sendCommand(sock, "JOIN " + channel);
        }
        // Exit command
        else if (userInput == "/exit")
        {
            std::cout << "Exiting..." << std::endl;
            isExit = true;
            sendCommand(sock, "QUIT " + channel + "\r\n");
            closesocket(sock);
            WSACleanup();
            ExitThread(0);
        }
        else
        {
            // Send a public message to the specified channel
            sendCommand(sock, "PRIVMSG " + channel + " :" + userInput + "\r\n");
        }
    }
    return 0;
}

/*
The main function that the program executes
*/
// Contributors: Siqi Peng, Jieshen Cai, Yao Yao
int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in6 serverAddr;
    std::string serverIP = "fc00:1337::17"; // IPv6 address
    std::string botName = "SuperBot";       // SuperBot name
    std::string channel = "#default";       // Default channel name
    int port = 6667;
    bool connected = false;

    // Parsing the command-line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--host" && i + 1 < argc)
        {
            serverIP = argv[++i];
        }
        else if (arg == "--port" && i + 1 < argc)
        {
            port = std::stoi(argv[++i]);
        }
        else if (arg == "--name" && i + 1 < argc)
        {
            botName = argv[++i];
        }
        else if (arg == "--channel" && i + 1 < argc)
        {
            channel = argv[++i];
        }
        else
        {
            std::cerr << "Unknown or missing argument: " << arg << std::endl;
            return 1;
        }
    }

    // Initialize WinSock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize WinSock" << std::endl;
        return 1;
    }

    // Creating Sockets
    if ((sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // Setting the server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin6_family = AF_INET6;
    inet(AF_INET6, serverIP.c_str(), &serverAddr.sin6_addr);
    serverAddr.sin6_port = htons(port);

    // connect to server
    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Connect failed" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    connected = true;

    if (connected)
    {
        std::cout << "Connect successfully!" << std::endl;
    }

    // Sending the NICK command
    sendCommand(sock, "NICK " + botName + "\r\n");

    // Sending the USER command
    sendCommand(sock, "USER " + botName + " 0 * :" + botName + "\r\n");

    // Join a channel
    sendCommand(sock, "JOIN " + channel + "\r\n");

    // Request the server to send a list of NAMES
    sendCommand(sock, "NAMES " + channel + "\r\n");

    // Thread parameters are created and passed to the thread
    ThreadParams *params = new ThreadParams;
    params->sock = sock;
    params->channel = channel;
    params->user = botName;

    // Create a user input thread
    HANDLE hThread = CreateThread(
        NULL,           // Default security property
        0,              // default stack size
        inputThread,    // Thread function
        (LPVOID)params, // Thread parameters
        0,              // Create flags by default
        NULL            // Receive thread identifier
    );

    if (hThread == NULL)
    {
        std::cerr << "Failed to create input thread" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Handling the server response
    handleServerResponse(sock, botName, channel);

    // Wait for the input thread to end
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    // Cleaning up thread parameters
    delete params;

    // Clean
    closesocket(sock);
    WSACleanup();
    return 0;
}
