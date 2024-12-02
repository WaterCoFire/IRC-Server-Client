/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_login
{
public:
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QTextEdit *name_edit;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QTextEdit *server_edit;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QTextEdit *port_edit;
    QWidget *widget_5;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QTextEdit *channel_edit;
    QFrame *frame_6;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *connect_btn;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *login)
    {
        if (login->objectName().isEmpty())
            login->setObjectName("login");
        login->resize(352, 485);
        widget = new QWidget(login);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(20, 30, 141, 41));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName("label");
        QFont font;
        font.setBold(true);
        label->setFont(font);

        horizontalLayout->addWidget(label);

        frame_2 = new QFrame(login);
        frame_2->setObjectName("frame_2");
        frame_2->setGeometry(QRect(10, 100, 320, 51));
        horizontalLayout_2 = new QHBoxLayout(frame_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_2 = new QLabel(frame_2);
        label_2->setObjectName("label_2");

        horizontalLayout_2->addWidget(label_2);

        name_edit = new QTextEdit(frame_2);
        name_edit->setObjectName("name_edit");
        name_edit->setMaximumSize(QSize(250, 16777215));
        name_edit->setFrameShape(QFrame::Box);

        horizontalLayout_2->addWidget(name_edit);

        widget_3 = new QWidget(login);
        widget_3->setObjectName("widget_3");
        widget_3->setGeometry(QRect(10, 170, 320, 51));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_3 = new QLabel(widget_3);
        label_3->setObjectName("label_3");

        horizontalLayout_3->addWidget(label_3);

        server_edit = new QTextEdit(widget_3);
        server_edit->setObjectName("server_edit");
        server_edit->setFrameShape(QFrame::Box);

        horizontalLayout_3->addWidget(server_edit);

        widget_4 = new QWidget(login);
        widget_4->setObjectName("widget_4");
        widget_4->setGeometry(QRect(10, 240, 320, 51));
        widget_4->setMaximumSize(QSize(320, 16777215));
        horizontalLayout_4 = new QHBoxLayout(widget_4);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        label_4 = new QLabel(widget_4);
        label_4->setObjectName("label_4");
        label_4->setMaximumSize(QSize(350, 16777215));

        horizontalLayout_4->addWidget(label_4);

        port_edit = new QTextEdit(widget_4);
        port_edit->setObjectName("port_edit");
        port_edit->setMaximumSize(QSize(250, 16777215));
        port_edit->setFrameShape(QFrame::Box);

        horizontalLayout_4->addWidget(port_edit);

        widget_5 = new QWidget(login);
        widget_5->setObjectName("widget_5");
        widget_5->setGeometry(QRect(10, 310, 320, 51));
        horizontalLayout_5 = new QHBoxLayout(widget_5);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        label_5 = new QLabel(widget_5);
        label_5->setObjectName("label_5");

        horizontalLayout_5->addWidget(label_5);

        channel_edit = new QTextEdit(widget_5);
        channel_edit->setObjectName("channel_edit");
        channel_edit->setFrameShape(QFrame::Box);

        horizontalLayout_5->addWidget(channel_edit);

        frame_6 = new QFrame(login);
        frame_6->setObjectName("frame_6");
        frame_6->setGeometry(QRect(-10, 410, 371, 81));
        frame_6->setFrameShape(QFrame::Box);
        horizontalLayout_6 = new QHBoxLayout(frame_6);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_2);

        pushButton = new QPushButton(frame_6);
        pushButton->setObjectName("pushButton");
        pushButton->setMaximumSize(QSize(16777215, 28));

        horizontalLayout_6->addWidget(pushButton);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_3);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        connect_btn = new QPushButton(frame_6);
        connect_btn->setObjectName("connect_btn");
        connect_btn->setMaximumSize(QSize(16777215, 28));

        horizontalLayout_6->addWidget(connect_btn);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer);


        retranslateUi(login);

        QMetaObject::connectSlotsByName(login);
    } // setupUi

    void retranslateUi(QWidget *login)
    {
        login->setWindowTitle(QCoreApplication::translate("login", "Form", nullptr));
        label->setText(QCoreApplication::translate("login", "Client infomation", nullptr));
        label_2->setText(QCoreApplication::translate("login", "Name:", nullptr));
        label_3->setText(QCoreApplication::translate("login", "ServerIP:", nullptr));
        label_4->setText(QCoreApplication::translate("login", "Port: ", nullptr));
        label_5->setText(QCoreApplication::translate("login", "Channel:", nullptr));
        pushButton->setText(QCoreApplication::translate("login", "Close", nullptr));
        connect_btn->setText(QCoreApplication::translate("login", "Connect", nullptr));
    } // retranslateUi

};

namespace Ui {
    class login: public Ui_login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
