/*
CONTRIBUTIONS OF THIS CODE FILE
Everyone in the group contributed to this code file.
Specific contributors of each function is provided in comments at the head of it.
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <ctime>
#include <algorithm>
#include <chrono>

#define PORT 6667

#define MAX_BUFFER_SIZE 1024
#define SERVER_NAME "server"
// server name is changeable

class IRCServer
{
public:
    /*Data for sockets*/
    std::set<std::string> channelWithoutBot;
    std::map<std::string, std::string> channel_topic;       // channel - topic
    std::map<int, std::string> user_nicknames;              // nicknames
    std::map<int, std::string> user_usernames;              // usernames
    std::map<int, std::string> user_realnames;              // real names
    std::map<int, std::string> user_hosts;                  // hosts
    std::map<int, std::set<std::string>> user_channels;     // channels
    std::map<std::string, std::set<int>> channel_users;     // users in every channel
    std::map<std::string, std::set<int>> channel_operators; // Operators(admin) in channel
    std::map<std::string, bool> channel_moderated;          // If the channel is moderated

    /*Constructor*/
    // Contributors: Shunyao Wang
    IRCServer() : server_fd(-1) {}

    /*Destructor*/
    // Contributors: Shunyao Wang
    ~IRCServer()
    {
        if (server_fd != -1)
        {
            close(server_fd);
        }
    }

    /*Start IRC server*/
    // Contributors: Jiangtao Ma, Jieshen Cai
    void start()
    {
        sockaddr_in6 address;
        int opt = 1;
        int addrlen = sizeof(address);

        // socket file descriptor
        if ((server_fd = socket(AF_INET6, SOCK_STREAM, 0)) == 0)
        {
            perror("SOCKET FAILED");
            exit(EXIT_FAILURE);
        }

        // attach socket to the port
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            perror("SETSOCKOPT FAILED");
            exit(EXIT_FAILURE);
        }

        address.sin6_family = AF_INET6;
        address.sin6_addr = in6addr_any; // in6addr_any: listen to any address
        address.sin6_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("BIND FAILED");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        // Debug
        std::cout << "Server is listening on port " << PORT << std::endl;

        /* Listening for new sockets */
        while (true)
        {
            int new_socket;
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                perror("ACCEPT ERROR");
                exit(EXIT_FAILURE);
            }
            std::thread(&IRCServer::handleClient, this, new_socket).detach();
        }
    }

private:
    int server_fd;

    int super_bot_socket = -1; // SuperBot socket
    bool super_bot_set = false;

    // Check validity of the nickname
    // Contributors: Yao Yao, Shunyao Wang
    bool is_invalid_nick(const std::string &nickname)
    {
        // Validity check of the nickname. No special characters allowed. No more than 12 characters.
        if (nickname.length() > 12 || nickname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != std::string::npos)
        {
            return true; // The "true" return value indicates that the nickname is INVALID
        }
        return false; // VALID nickname
    }

    /*Send a message to given socket
    called by all message sending functions e.g. broadcast, public message and private message*/
    // Contributors: Yao Yao, Siqi Peng
    int send_message(int client_socket, const std::string &message)
    {
        std::cout << "START SENDING MESSAGE..." << std::endl;
        int bytes_sent = send(client_socket, message.c_str(), message.length(), 0);
        if (bytes_sent == -1)
        {
            perror("SEND FAILED");
        }
        std::cout << "Sent message: " + message;
        return bytes_sent; // result value of send returned
    }

    /*broadcast to all channels (including sender itself)
    used during moments like joining a channel*/
    // Contributors: Jieshen Cai, Siqi Peng
    void broadcast_to_channel(const std::string &channel, const std::string &message)
    {
        std::cout << "START BROADCASTING..." << std::endl;
        for (const auto &user_socket : channel_users[channel])
        {
            std::cout << "SENDING TO: " << user_socket << std::endl;
            if (send_message(user_socket, message) == -1)
            {
                std::cerr << "Error sending to user socket: " << user_socket << std::endl;
            }
        }
    }

    /*
    sending public messages
    */
    // Contributors: Shunyao Wang, Siqi Peng
    void public_message(const std::string &channel, const std::string &message, int exclude_socket = -1)
    {
        std::cout << "PREPARE TO SEND PUBLIC MESSAGE: " << message << std::endl;

        // Check how many users are there in the channel
        const auto &users = channel_users[channel];
        std::cout << "START SENDING PUBLIC MESSAGE TO " << users.size() << " USERS..." << std::endl;

        if (users.size() > 1) // If there's two
        {
            // Send to everyone except for the sender itself
            for (const auto &user_socket : channel_users[channel])
            {
                // debug
                std::cout << "<<FOR>>: " << user_socket << ", exclude_socket: " << exclude_socket << std::endl;
                if (user_socket != exclude_socket && user_socket != super_bot_socket)
                {
                    std::cout << "SENDING TO: " << user_socket << std::endl;
                    if (send_message(user_socket, message) == -1)
                    {
                        std::cerr << "Error sending to user socket: " << user_socket << std::endl;
                    }
                }
            }
        }
        else if (users.size() == 1) // If only one user
        {
            // Directly send to itself
            auto user_socket = *users.begin(); // the only user
            std::string one_user_prompt = "[BE ADVISED] I am the only user currently in this channel.";
            if (send_message(user_socket, one_user_prompt) == -1)
            {
                std::cerr << "Error sending to user socket: " << user_socket << std::endl;
            }
        }
        else // exception: no sockets (unlikely though)
        {
            std::cerr << "NO SOCKETS FOUND!\n";
        }
    }

    /*handles messages that should be sent to the bot, e.g. !hello !slap etc.*/
    // Contributors: Yao Yao, Jieshen Cai
    void send_to_bot(const std::string &message)
    {
        std::cout << "SENDING TO BOT...\n";
        if (!super_bot_set)
        {
            std::cout << "ERROR BOT NOT SET!!\n";
            return;
        }

        std::cout << "Sending to SuperBot: " << super_bot_socket << std::endl;
        if (send_message(super_bot_socket, message) == -1)
        {
            std::cerr << "Error sending to user socket: " << super_bot_socket << std::endl;
        }
    }

    /* handles ping msg*/
    // Contributors: Jieshen Cai, Siqi Peng, Shunyao Wang
    void handle_ping(int client_socket, const std::string &message_id)
    {
        std::cout << "Handling: PING\n";

        std::string pong_message = ":" + std::string(SERVER_NAME) + " PONG " + SERVER_NAME + " :" + message_id + "\r\n";
        send_message(client_socket, pong_message);
    }

    /* handles join msg*/
    // Contributors: Siqi Peng, Jieshen Cai, Jiangtao Ma
    void handle_join(int client_socket, const std::string &orig_channel)
    {
        std::cout << "Handling: JOIN\n";

        std::string whole_message;

        std::string nickname = user_nicknames[client_socket];
        std::string channel = orig_channel;
        channel = channel.substr(0, channel.find("\r\n")); // deal with \r\n. (No more comments for similar code below)
        nickname = nickname.substr(0, nickname.find("\r\n"));

        // join message
        std::string join_msg = ":" + nickname + "!" + user_usernames[client_socket] + "@" + user_hosts[client_socket] + " JOIN #" + channel + "\r\n";

        if (channel_users.find(channel) == channel_users.end())
        {
            channel_users[channel] = {}; // int channel_users[channel]as empty set, avoid uninitialized user channel
        }

        // Add user
        for (const auto &user_socket : channel_users[channel])
        {
            if (user_nicknames[user_socket] == user_nicknames[client_socket] && user_socket != client_socket)
            {
                send_message(client_socket, "redundant nickname error");
                close(client_socket);
                return;
            }
        }

        // Remove the user from old channel if there exists
        std::string old_channel = "";
        if (user_channels[client_socket].size() != 0)
        {

            for (const std::string &channel : user_channels[client_socket])
            {
                old_channel += channel;
            }
            channel_users[old_channel].erase(client_socket);
            for (int socket : channel_users[old_channel])
            {
                if (socket != client_socket)
                {
                    handle_names(socket, old_channel + "\r\n");
                }
            }
        }

        // clear user's channel
        user_channels[client_socket].clear();
        user_channels[client_socket].insert(channel);
        channel_users[channel].insert(client_socket);

        // Check if the current channel has SuperBot set
        if (!super_bot_set)
        {
            channelWithoutBot.insert(channel);
        }
        else
        {
            for (const std::string &channel1 : channelWithoutBot)
            {
                channel_users[channel1].insert(super_bot_socket);
            }
            channel_users[channel].insert(super_bot_socket);
        }

        // Debugging statements to check the state of channels and users
        std::cout << "CLI SOCKET " << client_socket << " JOINED: #" << channel << std::endl;
        std::cout << "<<TOTAL USERS>> IN #" << channel << ": " << channel_users[channel].size() << std::endl;

        // Broadcast to other users in the channel
        broadcast_to_channel(channel, join_msg);
        whole_message += join_msg;
        for (int socket : channel_users[channel])
        {
            if (socket != client_socket)
            {
                handle_names(socket, orig_channel);
            }
        }
        // send 331 msg
        std::string no_topic_msg = ":" + std::string(SERVER_NAME) + " 331 " + nickname + " #" + channel + " :No topic is set\r\n";

        whole_message += no_topic_msg;
        // send_message(client_socket, no_topic_msg);

        // 353 msg
        std::string name_list = ":" + std::string(SERVER_NAME) + " 353 " + nickname + " = #" + channel + " :";
        for (const auto &user_socket : channel_users[channel])
        {
            name_list += user_nicknames[user_socket] + " ";
        }
        if (!name_list.empty() && name_list.back() == ' ') // Remove the last " "
        {
            name_list.erase(name_list.size() - 1);
        }
        name_list += "\r\n";

        whole_message += name_list;
        // send_message(client_socket, name_list);

        // 366 msg
        std::string end_names_list = ":" + std::string(SERVER_NAME) + " 366 " + nickname + " #" + channel + " :End of NAMES list\r\n";
        whole_message += end_names_list;
        // send_message(client_socket, end_names_list);

        send_message(client_socket, whole_message);
    }

    /* handle kick message */
    // Contributors: Yao Yao
    std::string handle_kick(int client_socket, const std::string &channel, const std::string &target)
    {
        std::cout << "Handling:KICK\n";

        int targetSocket = -1;
        for (const auto &user_socket : channel_users[channel])
        {

            if (user_nicknames[user_socket] == target)
            {
                targetSocket = user_socket;
                break;
            }
        }
        if (targetSocket == -1)
        {
            return "#" + channel + "/kick: " + target + " is not in the channel\r\n";
        }

        if (channel.find("default") == 0)
        {

            // send_message(client_socket, "/kick:you cannot kick " + target + " because this is default channel!\r\n");
            return "#" + channel + "/kick: you cannot kick " + target + " because this is default channel!\r\n";
        }
        if (targetSocket == super_bot_socket)
        {
            return "#" + channel + "/kick: you cannot kick " + target + "!\r\n";
        }

        channel_users[channel].erase(targetSocket);
        user_channels[targetSocket].erase(channel);
        channel_users["default"].insert(targetSocket);
        user_channels[targetSocket].insert("default");
        for (int socket : channel_users[channel])
        {
            handle_names(socket, channel + "\r\n");
        }
        for (int socket : channel_users["default"])
        {
            handle_names(socket, "default\r\n");
        }

        return ":" + user_nicknames[targetSocket] + "!" + user_usernames[targetSocket] + "@" + user_hosts[targetSocket] + "KICK" + "#" + channel + "/kick: kick successfully. " + user_nicknames[targetSocket] + " is moved to default channel...\r\n";
        // send_message(client_socket, "/kick:kick successfully!\r\n");
        // send_message(targetSocket, "/kick:you are moved to default channel...\r\n");
    }

    /* Handle TIME */
    // Contributors: Yao Yao, Shunyao Wang
    std::string handle_time(int client_socket)
    {
        std::cout << "Handling:TIME\n";

        // Get current time and display
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::tm *localTime = std::localtime(&currentTime);
        std::cout << "Server: Current time is " << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
        std::string channels = "";
        // Assemble message: Use !hello instead of PRIVMSG
        for (auto channel : user_channels[client_socket])
        {
            channels += channel;
        }
        return ":" + std::string(SERVER_NAME) + " 391 " + "#" + channels + "/time: Server local time is " + std::string(buffer) + "\r\n";
    }

    // Handle topic message
    // Contributors: Jiangtao Ma
    std::string handle_topic(int client_socket, const std::string &orig_channel)
    {
        std::cout << "Handling:TOPIC\n"; // debug
        // topic #default asd
        std::string channel = orig_channel.substr(6);

        // There are two format with !topic command:
        // if specific topic provided the topic will be set to that
        // if not it will be treated as checking the current topic
        if (channel.find(" ") == std::string::npos)
        {
            // checking
            channel = channel.substr(0, channel.find("\r\n"));
            channel = channel.substr(1);
            return ":" + std::string(SERVER_NAME) + " 332 " + "/topic: #" + channel + "'s topic is: " + channel_topic[channel] + "\r\n";
        }
        else
        {
            // setting
            std::string real_channel = channel.substr(0, channel.find(" "));
            real_channel = real_channel.substr(1);
            std::string topic = channel.substr(channel.find(" ") + 1);
            topic = topic.substr(0, topic.find("\r\n"));
            channel_topic[real_channel] = topic;

            return ":" + std::string(SERVER_NAME) + " 331 " + "/topic: #" + real_channel + "'s topic is set to : " + topic + "\r\n";
        }
    }

    // handle whois command
    // Contributors: Jieshen Cai
    std::string handle_whois(int client_socket, const std::string &orig_channel)
    {

        // !whois somebody
        std::string name = orig_channel.substr(6);
        if (name.find("\r\n") != std::string::npos)
        {

            name = name.substr(0, name.find("\r\n"));
        }

        std::cout << "Handling: WHOIS\n";

        // !whois command provides detailed information about the specified user
        // Find the target socket that represents the user
        int target_socket = find_socket_by_username(name);
        if (target_socket != -1)
        {
            // found
            std::string nick = user_nicknames[target_socket];
            std::string user = user_nicknames[target_socket];
            std::string host = user_hosts[target_socket];
            std::string channels = "";
            std::string client_channel = "";
            for (auto channel : user_channels[target_socket])
            {
                channels += channel;
            }
            for (auto channel : user_channels[client_socket])
            {
                client_channel += channel;
            }
            std::string msg = ":" + std::string(SERVER_NAME) + " 311 " + "#" + client_channel + "/whois: Nick: " + nick + "; " + "User: " + user + "@" + host + "; " + "Channel: " + channels + "\r\n";
            return msg;
        }
        else
        {
            // not found
            std::string client_channel = "";
            for (auto channel : user_channels[client_socket])
            {
                client_channel += channel;
            }
            std::string msg = ":" + std::string(SERVER_NAME) + "#" + client_channel + "/whois: " + name + " is not found in the server\r\n";

            return msg;
        }
    }
    // handle WHO msg
    // Contributors: Yao Yao, Jiangtao Ma, Jieshen Cai
    void handle_who(int client_socket, const std::string &orig_channel)
    {
        // Debug use
        std::cout << "Handling: WHO\n";

        std::string nickname = user_nicknames[client_socket];
        std::string channel = orig_channel;
        channel = channel.substr(0, channel.find("\r\n"));

        // send 352 msg to server
        for (const auto &user_socket : channel_users[channel])
        {
            std::string who_reply = ":" + std::string(SERVER_NAME) + " 352 " + nickname + " #" + channel + " " +
                                    user_usernames[user_socket] + " " + user_hosts[user_socket] + " " + SERVER_NAME + " " +
                                    user_nicknames[user_socket] + " H :0 " + user_realnames[user_socket] + "\r\n";
            send_message(client_socket, who_reply);
        }

        // ... and 315 msg
        size_t hash_pos = channel.find('#');
        if (hash_pos != std::string::npos)
        {
            channel = channel.substr(1);
        }
        std::string end_who_msg = ":" + std::string(SERVER_NAME) + " 315 " + nickname + " #" + channel + " :End of WHO list\r\n";
        send_message(client_socket, end_who_msg);
    }

    // handle MODE message
    // Contributors: Yao Yao, Siqi Peng, Shunyao Wang
    std::string handle_mode(int client_socket, const std::string &orig_message)
    {
        // For Debug use
        std::cout << "Handling: MODE" << std::endl;

        std::string message = orig_message;
        message = message.substr(orig_message.find("mode") + 5, message.find("\r\n"));

        // Parse Message
        size_t first_space = message.find(' ');
        if (first_space == std::string::npos)
        {
            std::cout << "Simple format detected during MODE handling - No space.\n";
            std::string default_reply = ":" + std::string(SERVER_NAME) + " 324 " + user_nicknames[client_socket] + " #" + message.substr(1) + " +\r\n";
            return default_reply; // Invalid message
        }

        std::string target = message.substr(0, first_space); // target
        std::string mode = message.substr(first_space + 1);  // mode

        // remove \r\n
        target = target.substr(0, target.find("\r\n"));
        mode = mode.substr(0, mode.find("\r\n"));

        std::cout << "PARSE RESULT: TARGET = " << target << ", MODE = " << mode << "\n";

        // Check whether the channel is user or channel
        if (target[0] == '#') // channel
        {
            std::string nickname = user_nicknames[client_socket];

            if (mode == "+m") // MODERATE: Mute all non-admin users in the channel
            {
                std::cout << "MODE SETTING: +m\n";

                channel_moderated[target] = true;
                std::string mode_reply = "/mode: Mute all in " + target + "!\r\n";
                return mode_reply;
            }
            else if (mode.substr(0, 2) == "-m")
            { // CANCEL MODERATE: Un-mute
                std::cout << "MODE SETTING: -m\n";

                channel_moderated[target] = false;
                std::string mode_reply = "/mode: Release mute all in " + target + "!\r\n";
                return mode_reply;
            }
            else if (mode.substr(0, 2) == "+o") // Add operator (admin)
            {
                std::cout << "MODE SETTING: +o\n";
                std::string username = mode.substr(3);               // Get username
                int user_socket = find_socket_by_username(username); // Find the socket by username
                if (user_socket != -1)
                {
                    channel_operators[target].insert(user_socket); // Add operator
                    std::string mode_reply = ":" + username + "!" + username + "@" + user_hosts[user_socket] + " MODE " + "/mode: Add " + user_nicknames[user_socket] + " to be operator in " + target + "!\r\n";
                    return mode_reply;
                }
                else
                {
                    std::string mode_reply = ":" + username + "!" + username + "@" + " MODE " + "/mode: " + username + "is not in " + target + "!\r\n";
                    return mode_reply;
                }
            }
            return "";
        }
        else // USER
        {
            std::string nickname = user_nicknames[client_socket];
            std::string mode_reply = ":" + std::string(SERVER_NAME) + " 221 " + nickname + " " + target + " +\r\n"; // Default Reply
            return mode_reply;
        }
    }

    /* Find socket by username */
    // Contributors: Siqi Peng
    int find_socket_by_username(const std::string &username)
    {
        for (const auto &pair : user_nicknames)
        {
            if (pair.second == username)
            {
                return pair.first; // relevant socket
            }
        }
        return -1; // Return -1 if not found
    }

    // handle NAMES message
    // Contributors: Jieshen Cai
    void handle_names(int client_socket, const std::string &orig_channel)
    {
        std::cout << "Handling: NAMES\n";

        std::string channel = orig_channel;
        channel = channel.substr(0, channel.find("\r\n"));
        std::string nickname = user_nicknames[client_socket];
        // 353 message
        std::string names_reply = ":" + std::string(SERVER_NAME) + " 353 " + nickname + " = #" + channel + " :";

        // all the names in the channel
        for (const auto &user_socket : channel_users[channel])
        {
            names_reply += user_nicknames[user_socket] + " ";
        }
        // remove the last space character
        if (!names_reply.empty() && names_reply.back() == ' ')
        {
            names_reply.erase(names_reply.size() - 1);
        }
        // send 366 message
        names_reply += "\r\n:" + std::string(SERVER_NAME) + " 366 " + nickname + " #" + channel + " :End of NAMES list\r\n";

        send_message(client_socket, names_reply);
    }

    // handle QUIT message (User leaves)
    // Contributors: Shunyao Wang, Jiangtao Ma
    void handle_quit(int client_socket, const std::string &quit_message = "")
    {
        std::cout << "Handling: QUIT" << "\n";
        // get the nickname of the user
        std::string nickname = user_nicknames[client_socket];

        if (nickname.empty())
        {
            nickname = "UNKNOWN_NICKNAME";
        }

        // assemble the broadcast message
        std::string quit_msg = ":" + nickname + " QUIT :" + quit_message + "\r\n";

        std::string channels = "";
        for (const auto &channel : user_channels[client_socket])
        {
            // broadcast this quit message
            broadcast_to_channel(channel, quit_msg);
            channels += channel;
            // remove from the channel
            channel_users[channel].erase(client_socket);

            // delete the channel if there are no longer any users in it
            if (channel_users[channel].empty())
            {
                channel_users.erase(channel);
            }
        }

        // erase any relevant data of this user
        user_nicknames.erase(client_socket);
        user_usernames.erase(client_socket);
        user_realnames.erase(client_socket);
        user_hosts.erase(client_socket);
        user_channels.erase(client_socket);
        for (int socket : channel_users[channels])
        {
            if (socket != client_socket)
            {
                handle_names(socket, channels + "\r\n");
            }
        }
        // disconnect
        close(client_socket);

        // COUT FOR DEBUG USE
        std::cout << "User " << nickname << " has left the server." << std::endl;
    }

    // handle PRIVMSG aka message
    // Contributors: Jieshen Cai, Siqi Peng
    void handle_privmsg(int client_socket, const std::string &target, const std::string &message)
    {
        std::cout << "START HANDLING THIS MESSAGE." << std::endl;
        std::string nickname = user_nicknames[client_socket];
        std::string privmsg;

        // the first character is #
        // public message
        if (target[0] == '#')
        {
            // Sending messages starting with '!': Send it to the bot (if exists) instead
            if (message.substr(0, 1) == "!")
            {
                std::string bot_command = message.substr(1);
                std::cout << "<<! MESSAGE>>: DETECTED\n";
                if (!super_bot_set)
                {
                    std::cout << "<<! MESSAGE>>: ERROR NO BOT SET!\n";
                    send_message(client_socket, ":" + std::string(SERVER_NAME) + "ERROR: No SuperBot detected!\r\n");
                    return;
                }
                if (bot_command.find("kick") != std::string::npos)
                {
                    std::string orig_channel = bot_command;
                    std::string channels = "";
                    for (auto channel : user_channels[client_socket])
                    {
                        if (channel.find("#"))
                        {
                            channel = channel.substr(0);
                        }
                        channels += channel;
                    }

                    std::string target = orig_channel.substr(orig_channel.find("kick") + 5, orig_channel.find("\r\n"));
                    target = target.substr(0, target.find("\r\n"));
                    std::cout << target + '\n';
                    std::string msg = handle_kick(client_socket, channels, target);
                    send_to_bot(msg);
                    return;
                }
                else if (bot_command.find("time") != std::string::npos)
                {
                    std::string msg = handle_time(client_socket);
                    send_to_bot(msg);
                }
                else if (bot_command.find("mode") != std::string::npos)
                {
                    send_to_bot(handle_mode(client_socket, bot_command));
                }
                else if (bot_command.find("whois") != std::string::npos)
                {
                    send_to_bot(handle_whois(client_socket, bot_command));
                }
                else if (bot_command.find("topic") != std::string::npos)
                {
                    send_to_bot(handle_topic(client_socket, bot_command));
                }
                std::cout << "<<! MESSAGE>>: PREPARING TO SEND TO BOT...\n";
                std::string channels = "";
                // Assemble message: Use !hello instead of PRIVMSG
                for (auto channel : user_channels[client_socket])
                {
                    channels += channel;
                }

                privmsg = ":" + nickname + "!" + user_nicknames[client_socket] + "@" + user_hosts[client_socket] + " /" + bot_command + target + " :" + message + "\r\n";
                std::cout << privmsg + '\n';
                send_to_bot(privmsg);
                return;
            }
            std::string channels = "";
            for (auto channel : user_channels[client_socket])
            {
                channels += channel;
            }
            std::cout << "Handling: PUBLIC" << std::endl;
            // Sending to a channel
            privmsg = ":" + nickname + "!" + user_nicknames[client_socket] + "@" + user_hosts[client_socket] + " PRIVMSG " + target + " :" + message + "\r\n";
            public_message(target.substr(1), privmsg, client_socket);
        }
        else
        {
            // Otherwise private message
            std::cout << "Handling: PRIVATE" << std::endl;
            // Private message to a user
            int target_socket = -1;
            for (const auto &[sock, nick] : user_nicknames)
            {
                if (nick == target)
                {
                    target_socket = sock;
                    break;
                }
            }
            if (target_socket != -1)
            {
                privmsg = ":" + nickname + "!" + user_nicknames[client_socket] + "@" + user_hosts[client_socket] + " PRIVMSG " + target + " :" + message + "\r\n";
                send_message(target_socket, privmsg);
            }
        }
    }

    // Handle message from client, HexChat or client.exe from Windows VM
    // Contributors: Yao Yao, Jieshen Cai, Siqi Peng, Jiangtao Ma, Shunyao Wang
    void handleClient(int client_socket)
    {
        char buffer[MAX_BUFFER_SIZE] = {0};
        std::string user_name;
        std::string channel;
        std::string real_name;
        bool nick_set = false;

        // Recv msg
        while (true)
        {
            memset(buffer, 0, sizeof(buffer));
            int bytes_read = read(client_socket, buffer, MAX_BUFFER_SIZE);
            if (bytes_read <= 0)
            {
                close(client_socket);
                return;
            }
            buffer[bytes_read] = '\0'; // Add end signal
            std::string msg = std::string(buffer);

            // Debug use: print it
            std::cout << "Received from client: " << msg << std::endl;

            // Deal with NICK first
            if (msg.substr(0, 5) == "NICK ")
            {
                std::cout << "Handling: NICK\n";
                // HexChat usually sends NICK and USER msg together so proper handling is necessary
                std::string nickname;
                std::string possible_user_part;
                bool contain_user_part = false;

                // LOOK FOR NICK AND USER
                size_t nick_pos = msg.find("NICK ");
                size_t user_pos = msg.find("USER ");

                // Starting position AFTER "NICK "
                size_t start_pos = nick_pos + 5;

                // Look if there is "USER " after "NICK "
                if (user_pos != std::string::npos && user_pos > nick_pos)
                {
                    // If found then obtain the NICK msg as well
                    nickname = msg.substr(start_pos, user_pos - start_pos);
                    possible_user_part = msg.substr(user_pos);
                    contain_user_part = true;
                }
                else
                {
                    // If not found then get all after NICK as nickname
                    nickname = msg.substr(start_pos);
                }
                nickname = nickname.substr(0, nickname.find("\r\n"));

                // Check for invalid nicknames (call the relevant function)
                if (is_invalid_nick(nickname))
                {
                    std::cout << "Invalid nickname detected: " << nickname << std::endl; // FOR DEBUG USE
                    std::string error_msg = "ERROR: Invalid nickname. Please choose another nickname.\r\n";
                    send(client_socket, error_msg.c_str(), error_msg.length(), 0);
                    std::cout << "The socket is closed." << std::endl;
                    // Disconnect
                    close(client_socket);
                    return; // return in advance
                }

                // check if the new client is a SuperBot
                if (nickname == "SuperBot")
                {
                    std::cout << "Accepting the SuperBot!\n";
                    super_bot_socket = client_socket;
                    super_bot_set = true;
                }
                else
                {
                    std::cout << "Accepting new NORMAL USER: " << nickname << std::endl;
                }

                user_nicknames[client_socket] = nickname;

                if (contain_user_part) // Found USER command
                {
                    std::cout << "Handling: USER (in the same message with NICK)\n";
                    // USER <username> <hostname> <servername> :<realname>
                    size_t space1 = possible_user_part.find(' ', 5);
                    size_t space2 = possible_user_part.find(' ', space1 + 1);
                    size_t colon = possible_user_part.find(':', space2 + 1);

                    if (space1 != std::string::npos && space2 != std::string::npos && colon != std::string::npos)
                    {
                        real_name = possible_user_part.substr(colon + 1);
                        real_name = real_name.substr(0, real_name.find("\r\n"));

                        std::cout << "Received USER command with real name: " << real_name << std::endl;
                    }
                    else
                    {
                        std::string error_msg = "ERROR! Invalid USER command\r\n";
                        send(client_socket, error_msg.c_str(), error_msg.length(), 0);
                    }
                }
            }

            // Deal with CAP
            if (msg.substr(0, 7) == "CAP LS ")
            {
                std::string cap_msg = ":server CAP * LS :\r\n";
                send(client_socket, cap_msg.c_str(), cap_msg.length(), 0);
            }
            else if (msg.substr(0, 7) == "CAP END")
            {
                std::cout << "Server: CAP END Received.\n";
            }
            // MODE
            else if (msg.substr(0, 5) == "MODE ")
            {
                std::string mode_msg = msg.substr(0, msg.find("\r\n"));
                std::cout << "Handling: MODE" << std::endl;

                std::string channel = mode_msg.substr(5);
                channel = channel.substr(0, channel.find("\r\n"));
                // send 324 message
                std::string nickname = user_nicknames[client_socket];
                std::string mode_reply = ":" + std::string(SERVER_NAME) + " 324 " + nickname + " #" + channel + " +\r\n";
                send_message(client_socket, mode_reply);
                size_t who_pos = msg.find("WHO ");
                // If found "WHO " after "MODE "
                if (who_pos != std::string::npos)
                {
                    std::string who_msg = msg.substr(who_pos);
                    who_msg = who_msg.substr(0, who_msg.find("\r\n"));

                    handle_who(client_socket, who_msg.substr(5));
                }
            }

            // Deal with NAMES
            else if (msg.substr(0, 6) == "NAMES ")
            {
                handle_names(client_socket, msg.substr(7)); // without #
            }
            // Deal with WHO
            else if (msg.substr(0, 4) == "WHO ")
            {
                handle_who(client_socket, msg.substr(4));
            }

            // Deal with USER command
            else if (msg.substr(0, 5) == "USER ")
            {
                std::cout << "Handling: USER\n";
                // client.exe could send USER and JOIN together as well, so like before, we should handle them here
                std::string user_msg;
                std::string possible_join_part;
                bool contain_join_part = false;

                // look for "USER " and "JOIN "
                size_t user_pos = msg.find("USER ");
                size_t join_pos = msg.find("JOIN ");

                // If found "JOIN " after "USER "
                if (join_pos != std::string::npos && join_pos > user_pos)
                {
                    user_msg = msg.substr(user_pos, join_pos - user_pos);
                    possible_join_part = msg.substr(join_pos);
                    contain_join_part = true;
                }
                else
                {
                    // NOT FOUND
                    user_msg = msg.substr(user_pos);
                }

                // USER <username> <hostname> <servername> :<realname>
                size_t space1 = user_msg.find(' ', 5);
                size_t space2 = user_msg.find(' ', space1 + 1);
                size_t colon = user_msg.find(':', space2 + 1);

                if (space1 != std::string::npos && space2 != std::string::npos && colon != std::string::npos)
                {
                    real_name = user_msg.substr(colon + 1);
                    real_name = real_name.substr(0, real_name.find("\r\n"));

                    std::cout << "Received USER command with real name: " << real_name << std::endl;
                }
                else
                {
                    std::string error_msg = "ERROR! Invalid USER command\r\n";
                    send(client_socket, error_msg.c_str(), error_msg.length(), 0);
                }

                if (contain_join_part)
                { // FOUND JOIN msg with USER
                    std::cout << "Found JOIN in the same message with USER.\n";
                    handle_join(client_socket, possible_join_part.substr(6)); // Channel without '#'
                }
            }
            // Handle JOIN
            else if (msg.substr(0, 5) == "JOIN ")
            {
                handle_join(client_socket, msg.substr(6)); // Channel without '#'
            }
            // Message
            else if (msg.substr(0, 8) == "PRIVMSG ")
            {
                // PRIVMSG (text message)
                std::cout << "RECEIVED A MESSAGE: " << msg << std::endl;

                // Find the first space (the one after PRIVMSG)
                size_t first_space = msg.find(' ');

                // the second one (after the target)
                size_t second_space = msg.find(' ', first_space + 1);

                // get the target (receipt)
                std::string target = msg.substr(first_space + 1, second_space - first_space - 1);

                // get text message body (after the target)
                std::string message_body = msg.substr(second_space + 1);

                // remove the ":" at the beginning of the message, if there is one
                if (!message_body.empty() && message_body[0] == ':')
                {
                    message_body.erase(0, 1);
                }

                // Debug use
                std::cout << "IT SHOULD BE SENT TO: " << target << ", TEXT: " << message_body << std::endl;

                // Check if the target is a channel and if it's moderated
                if (target[0] == '#' && channel_moderated[target]) // Channel is set to moderate mode
                {
                    // Check if the user is an operator
                    if (channel_operators[target].find(client_socket) == channel_operators[target].end())
                    {
                        std::cout << "WARNING: Channel currently moderated and the user is not an operator, message not sent." << std::endl;
                        continue; // NO SENDING
                    }
                }

                std::cout << "Channel unrestricted or the user is operator, message will be sent." << std::endl;
                handle_privmsg(client_socket, target, message_body);
            }

            // PING message handling
            else if (msg.substr(0, 5) == "PING ")
            {
                handle_ping(client_socket, msg.substr(5));
            }

            // QUIT message handling
            else if (msg.substr(0, 5) == "QUIT ")
            {
                std::string quit_message = msg.substr(5);
                handle_quit(client_socket, quit_message);
                break;
            }

            // Otherwise, an unknown message, print it for debug use
            else
            {
                std::cout << "UNKNOWN MSG: " << msg << "\n";
            }
        }
    }
};

/*main() function*/
// Contributors: Yao Yao
int main()
{
    // START SERVER
    IRCServer server;
    server.start();
    return 0;
}