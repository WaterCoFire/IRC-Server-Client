#include "widget.h"
#include "login.h"
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{

    QString serverIP, name, port, channel;

    for (int i = 1; i < argc; ++i)
    {
        QString arg = argv[i];

        if (arg == "--serverIP" && i + 1 < argc)
        {
            serverIP = argv[++i]; // get serverIP value
        }
        else if (arg == "--name" && i + 1 < argc)
        {
            name = argv[++i]; // get nickname value
        }
        else if (arg == "--port" && i + 1 < argc)
        {
            port = argv[++i]; // get port value
        }
        else if (arg == "--channel" && i + 1 < argc)
        {
            channel = argv[++i]; // get channel value
        }
    }

    QApplication a(argc, argv);
    mainWindow mainWindow;
    mainWindow.setLoginParameters(serverIP, name, port, channel);
    mainWindow.loginWindow->show();
    return a.exec();
}
