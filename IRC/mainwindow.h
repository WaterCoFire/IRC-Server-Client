/*
CONTRIBUTIONS OF THIS CODE FILE
Yao Yao
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "widget.h"
#include "login.h"
#include <QWidget>

namespace Ui {
class mainWindow;
}

class mainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit mainWindow(QWidget *parent = nullptr);
    ~mainWindow();
    login *loginWindow;
    Widget *chatRoom;
    void setLoginParameters(const QString &serverIP, const QString &name, const QString &port, const QString &channel);
private slots:
    void showLoginWindow();
    void onConnectToServer(const QString &server,const QString &name, const QString &port, const QString &channel);

private:
    Ui::mainWindow *ui;

};

#endif // MAINWINDOW_H
