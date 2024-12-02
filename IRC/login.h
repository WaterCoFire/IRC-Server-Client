/*
CONTRIBUTIONS OF THIS CODE FILE
Yao Yao, Jieshen Cai
*/

#ifndef LOGIN_H
#define LOGIN_H
#include <QTcpSocket>
#include <QWidget>
#include "widget.h"
namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();
    void setInitialValues(const QString &serverIP, const QString &name, const QString &port, const QString &channel);

signals:
    // Signal used to notify that the connection is successful and entering the chat room
    void connectToServer(const QString &serverIP, const QString &name, const QString &port, const QString &channel);

private slots:
    void onConnectClicked();
private:
    Ui::login *ui;
};

#endif // LOGIN_H
