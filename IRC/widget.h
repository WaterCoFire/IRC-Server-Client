#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    int connectToServer(const QString &serverIP,const QString &name, const QString &port, const QString &channel);
signals:
    // switch back to the login window
    void switchToLogin();
private slots:
    // Handling user sending message
    void sendMessage(const QString &serverIP,const QString &name, const QString &port);
    void updateCountdown();
    // Deal with the response message
    void handleServerResponse();
    void exiting();
private:
    QString serverIP;
    QString userName;
    QString port;
    QString channelName;
    Ui::Widget *ui;
    QTcpSocket *socket;
    QTimer *countdownTimer;
    int countdownValue;
    std::vector<std::string> currentUsers;
    int count;
};
#endif // WIDGET_H
