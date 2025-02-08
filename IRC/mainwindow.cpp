#include "mainwindow.h"
#include "ui_mainwindow.h"

mainWindow::mainWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::mainWindow)
{
    ui->setupUi(this);
    loginWindow = new login();
    chatRoom = new Widget();
    chatRoom->hide();    // Hide Chat Room Window
    loginWindow->show(); // Display Login Window

    // Connect to signal slots
    connect(loginWindow, &login::connectToServer, this, &mainWindow::onConnectToServer);
    connect(chatRoom, &Widget::switchToLogin, this, &mainWindow::showLoginWindow);
}

// Set login parameters
void mainWindow::setLoginParameters(const QString &serverIP, const QString &name, const QString &port, const QString &channel)
{
    loginWindow->setInitialValues(serverIP, name, port, channel);
}

// Action on connecting to server
void mainWindow::onConnectToServer(const QString &server, const QString &name, const QString &port, const QString &channel)
{
    int res = chatRoom->connectToServer(server, name, port, channel);

    if (res == 0)
    {
        loginWindow->hide();
        chatRoom->show();
    }
    else
    {
        return;
    }
}

// Display Login Window
void mainWindow::showLoginWindow()
{
    loginWindow->show();
}
mainWindow::~mainWindow()
{
    delete ui;
}
