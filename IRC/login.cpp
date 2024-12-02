/*
CONTRIBUTIONS OF THIS CODE FILE
Siqi Peng, Jieshen Cai
*/

#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent)
    : QWidget(parent), ui(new Ui::login)
{
    ui->setupUi(this);
    // Button click event
    connect(ui->connect_btn, &QPushButton::clicked, this, &login::onConnectClicked);
}

// Set inital values on Login
void login::setInitialValues(const QString &serverIP, const QString &name, const QString &port, const QString &channel)
{
    if (!serverIP.isEmpty())
    {
        ui->server_edit->setText(serverIP);
    }
    else
    {
        ui->server_edit->setText("fc00:1337::17");
    }
    if (!name.isEmpty())
    {
        ui->name_edit->setText(name);
    }
    else
    {
        ui->name_edit->setText("SuperBot");
    }
    if (!port.isEmpty())
    {
        ui->port_edit->setText(port);
    }
    else
    {
        ui->port_edit->setText("6667");
    }
    if (!channel.isEmpty())
    {
        ui->channel_edit->setText(channel);
    }
    else
    {
        ui->channel_edit->setText("#default");
    }
}

// Action on connect button is clicked
void login::onConnectClicked()
{
    QString server = ui->server_edit->toPlainText();
    QString name = ui->name_edit->toPlainText();
    QString port = ui->port_edit->toPlainText();
    QString channel = ui->channel_edit->toPlainText();
    if (name.isEmpty())
    {
        QMessageBox error;
        error.setIcon(QMessageBox::Critical);
        error.setWindowTitle("Error");
        error.setText("Invalid Name!");
        error.exec();
        return;
    }
    emit connectToServer(server, name, port, channel);
}
login::~login()
{
    delete ui;
}
