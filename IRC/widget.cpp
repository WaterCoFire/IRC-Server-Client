#include "widget.h"
#include "./ui_widget.h"
#include <QHostAddress>
#include <QDebug>
#include <QString>
#include <QDateTime>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <QTimer>
#include <QThread>
Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget),
      socket(new QTcpSocket(this)),
      countdownTimer(new QTimer(this)), // Initialize timer
      countdownValue(5)                 // Countdown default time value
{
    ui->setupUi(this);
    ui->textBrowser->setReadOnly(true);
    ui->tableWidget->setColumnCount(1);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Online Users");

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Connect the send button with sendMessage slot function
    connect(ui->btn_send, &QPushButton::clicked, this, [this]()
            {
        QString serverIP = this->serverIP;
        QString name = this->userName;
        QString port = this->port;
        QString channel = this->channelName;
        // Call the sendMessage() with parameters
        sendMessage(serverIP, name, port); });
    connect(ui->btn_exit, &QPushButton::clicked, this, &Widget::exiting);

    connect(socket, &QTcpSocket::readyRead, this, &Widget::handleServerResponse);
    // Connect to timer signal slot for updating countdown
    connect(countdownTimer, &QTimer::timeout, this, &Widget::updateCountdown);
}

// Server connection
int Widget::connectToServer(const QString &serverIP, const QString &name, const QString &port, const QString &channel)
{

    QHostAddress address(serverIP);
    socket->connectToHost(address, port.toInt());
    if (socket->waitForConnected(5000))
    {
        this->serverIP = serverIP;
        this->userName = name;
        this->channelName = channel;
        this->port = port;
        qDebug() << "Hello, Qt Console Output!";
        ui->textBrowser->append("Connected to server.");
        QStringList commands;
        commands << QString("NICK %1\r\n").arg(name)
                 << QString("USER %1 0 * :%2\r\n").arg(name, name)
                 << QString("JOIN %1\r\n").arg(channel)
                 << QString("NAMES %1\r\n").arg(channel);

        // Send NICK and USER command
        for (int var = 0; var < commands.size(); ++var)
        {
            QTimer::singleShot(var * 1000, [this, commands, var]()
                               { socket->write(commands[var].toUtf8()); });
        }
        socket->write(QString("NICK %1\r\n").arg(name).toUtf8());

        socket->write(QString("USER %1 0 * :%2\r\n").arg(name, name).toUtf8());
        socket->write(QString("JOIN %1\r\n").arg(channel).toUtf8());
        socket->write(QString("NAMES %1\r\n").arg(channel).toUtf8());
        return 0;
    }
    else
    {
        QMessageBox::critical(this, "Connection Error", "Failed to connect to server.");
        return 1;
    }
}

/*
Get an random fun fact
*/
std::string getRandomFunFact()
{
    std::ifstream file("funfacts.txt"); // file name
    std::vector<std::string> facts;
    std::string line;

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
get a random user
*/
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
void Widget::exiting()
{
    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(QString("QUIT %1\r\n").arg(channelName).toUtf8());
        socket->close();
        qApp->quit();
    }
}
void Widget::sendMessage(const QString &serverIP, const QString &name, const QString &port)
{
    QString userInput = ui->textEdit->toPlainText(); // Get user input of QLineEdit
    ui->textEdit->clear();                           // Empty input box
    // Check if the input is private message
    if (userInput.startsWith("/msg "))
    {
        // If it is
        QString privateMessage = userInput.mid(5); // remove "/msg "
        QStringList parts = privateMessage.split(' ', Qt::SkipEmptyParts);
        if (parts.size() > 1)
        {
            QString targetUser = parts[0];
            QString message = privateMessage.mid(targetUser.length() + 1); // get message
            // send PRIVMSG
            socket->write(QString("PRIVMSG %1 :%2\r\n").arg(targetUser, message).toUtf8());
        }
        else
        {
            ui->textBrowser->append("Invalid private message format. Use: /msg <user> <message>");
        }
    }
    // Handling exit command
    else if (userInput == "/exit")
    {
        ui->textBrowser->append("Exiting...");
        socket->write(QString("QUIT %1\r\n").arg(channelName).toUtf8());
        socket->close();
        qApp->quit(); // Quit the application
    }
    // Handling join command
    else if (userInput.startsWith("/join"))
    {
        if (!userInput.contains('#'))
        {
            ui->textBrowser->append("ERROR: you should add '#' before channel.");
            return;
        }
        QString channel = userInput.mid(userInput.indexOf("#"));
        channelName = channel;
        socket->write(QString("JOIN %1\r\n").arg(channel).toUtf8());
    }
    // Public message otherwise
    else
    {
        socket->write(QString("PRIVMSG %1 :%2\r\n").arg(channelName, userInput).toUtf8());
    }

    // Display to the chat area
    ui->textBrowser->append(name + ": " + userInput);
}

// Handle response from server
void Widget::handleServerResponse()
{
    // Read the response from server
    QByteArray buffer = socket->readAll();
    std::string response = buffer.toStdString();

    // HANDLING PING
    if (response.find("PING :") == 0)
    {
        std::string pongResponse = "PONG " + response.substr(5) + "\r\n";
        socket->write(QString::fromStdString(pongResponse).toUtf8());
        return;
    }

    // Check for nickname error
    if (response.find("nickname error") != std::string::npos)
    {
        ui->textBrowser->append("Redundant nickname error");
        socket->disconnectFromHost();
        return;
    }

    // HANDLING WHOIS
    if (response.find(" 311 ") != std::string::npos)
    {
        ui->textBrowser->append(QString::fromStdString("WHOIS Info: " + response));
    }

    // HANDLING PRIVMSG
    size_t privmsgPos = response.find(" PRIVMSG ");
    if (privmsgPos != std::string::npos)
    {
        size_t senderEnd = response.find('!');
        std::string sender = response.substr(1, senderEnd - 1);

        size_t targetStart = privmsgPos + 9;
        size_t targetEnd = response.find(" :", targetStart);
        std::string target = response.substr(targetStart, targetEnd - targetStart);

        std::string message = response.substr(targetEnd + 2);
        std::string channel1 = "";
        if (response.find("#") != std::string::npos)
        {
            channel1 = response.substr(response.find("#"));
            channel1 = channel1.substr(0, channel1.find("\r\n"));
            channel1 = channel1.substr(0, channel1.find(" :"));
        }

        ui->textBrowser->append(QString::fromStdString(sender + ": " + message));

        if (userName == "SuperBot")
        {
            std::string funFact = getRandomFunFact();
            std::string responseMessage = "PRIVMSG " + sender + " :" + funFact + "\r\n";
            socket->write(QString::fromStdString(responseMessage).toUtf8());
        }
    }

    // HANDLING NAMES
    size_t namesPos = response.find(" 353 ");
    if (namesPos != std::string::npos)
    {
        size_t usersStart = response.find(" :", namesPos);
        size_t usersEnd = response.find("\r\n", usersStart);
        if (usersStart != std::string::npos && usersEnd != std::string::npos)
        {
            std::string userList = response.substr(usersStart + 2, usersEnd - usersStart - 2);
            currentUsers.clear();
            std::stringstream ss(userList);
            std::string user;
            while (ss >> user)
            {
                currentUsers.push_back(user);
            }
            // Update active users list to tableWidget
            ui->tableWidget->clearContents();
            ui->tableWidget->setRowCount(0); // empty table content

            // traverse through currentUsers and update table
            for (size_t i = 0; i < currentUsers.size(); ++i)
            {
                ui->tableWidget->insertRow(i); // new row
                QTableWidgetItem *userItem = new QTableWidgetItem(QString::fromStdString(currentUsers[i]));
                ui->tableWidget->setItem(i, 0, userItem);
            }
            count = currentUsers.size();
            ui->label->setText("Online: " + QString::number(count));
        }
    }

    // Handle extra commands
    // TIME
    size_t timePos = response.find("/time: ");
    if (timePos != std::string::npos)
    {
        std::string channel1 = response.substr(response.find("#"));
        channel1 = channel1.substr(0, channel1.find("\r\n"));
        ui->textBrowser->append(QString::fromStdString(response.substr(timePos + 7)));
        socket->write(QString::fromStdString("PRIVMSG " + channel1 + " :" + response.substr(timePos + 7)).toUtf8());
    }

    // KICK
    size_t kickPos = response.find("/kick: ");
    if (kickPos != std::string::npos)
    {
        std::string channel1 = response.substr(response.find("#"), response.find("/"));
        ui->textBrowser->append(QString::fromStdString(response.substr(kickPos + 7)));
        socket->write(QString::fromStdString("PRIVMSG " + channel1 + " :" + response.substr(kickPos + 7)).toUtf8());
    }

    // MODE
    size_t modePos = response.find("/mode: ");
    if (modePos != std::string::npos)
    {
        std::string channel1 = response.substr(response.find("#"), response.find("!"));
        ui->textBrowser->append(QString::fromStdString(response.substr(modePos + 7)));
        socket->write(QString::fromStdString("PRIVMSG " + channel1 + " :" + response.substr(modePos + 7)).toUtf8());
    }

    // HELLO
    size_t helloPos = response.find("/hello");
    if (helloPos != std::string::npos)
    {
        std::string sender = response.substr(1, response.find('!') - 1);
        ui->textBrowser->append(QString::fromStdString("Hello, " + sender));
        socket->write(QString::fromStdString("PRIVMSG " + sender + " :Hello, " + sender + "!\r\n").toUtf8());
    }

    // TOPIC
    size_t topicPos = response.find("/topic: ");
    if (topicPos != std::string::npos)
    {
        ui->textBrowser->append(QString::fromStdString(response.substr(topicPos + 8)));
        std::string channel1 = response.substr(response.find("#"));
        channel1 = channel1.substr(0, channel1.find("\r\n"));
        channel1 = channel1.substr(0, channel1.find("'s"));
        socket->write(QString::fromStdString("PRIVMSG " + channel1 + " :" + response.substr(topicPos + 8)).toUtf8());
    }

    // WHOIS
    size_t whoamiPos = response.find("/whois: ");
    if (whoamiPos != std::string::npos)
    {
        std::string channel1 = response.substr(response.find("#"));
        channel1 = channel1.substr(0, channel1.find("\r\n"));
        channel1 = channel1.substr(0, channel1.find("/"));
        ui->textBrowser->append(QString::fromStdString(response.substr(whoamiPos + 8)));
        socket->write(QString::fromStdString("PRIVMSG " + channel1 + " :" + response.substr(whoamiPos + 8)).toUtf8());
    }

    // SLAP
    size_t slapPos = response.find("/slap");
    if (slapPos != std::string::npos)
    {
        std::string sender = response.substr(1, response.find('!') - 1);
        std::string target = response.substr(slapPos + 7);
        std::string channel1 = response.substr(response.find("#"));
        channel1 = channel1.substr(0, channel1.find("\r\n"));
        channel1 = channel1.substr(0, channel1.find(" :"));

        if (target.empty())
        {
            target = getRandomUser(currentUsers, sender, "SuperBot");
        }

        if (std::find(currentUsers.begin(), currentUsers.end(), target) != currentUsers.end())
        {
            ui->textBrowser->append(QString::fromStdString("Bot slaps " + target));
            socket->write(QString::fromStdString("PRIVMSG " + channel1 + " :slaps " + target + "!!!\r\n").toUtf8());
        }
        else
        {
            ui->textBrowser->append(QString::fromStdString("User not found, slapping " + sender));
            socket->write(QString::fromStdString("PRIVMSG " + channel1 + " :slaps " + sender + " himself!!!\r\n").toUtf8());
        }
    }

    size_t errPos = response.find("ERROR");
    if (errPos != std::string::npos)
    {
        ui->textBrowser->append(QString::fromStdString("Server error: " + response.substr(errPos)));
    }
}

// Countdown update
void Widget::updateCountdown()
{
    if (countdownValue > 0)
    {
        ui->textBrowser->append(QString("Returning to login in %1 seconds...").arg(countdownValue));
        countdownValue--;
    }
    else
    {
        countdownTimer->stop(); // Stop timer
        this->close();          // close the current ChatRoom window
        // Shut down the ChatRoom window and return to Login
        emit switchToLogin(); // Send a signal to notify the main window to switch back to the login window
    }
}
Widget::~Widget()
{
    delete ui;
}
