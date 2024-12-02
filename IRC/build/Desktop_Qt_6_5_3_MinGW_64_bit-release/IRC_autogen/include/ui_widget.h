/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QHBoxLayout *horizontalLayout_6;
    QWidget *widget_7;
    QHBoxLayout *horizontalLayout_4;
    QWidget *widget_5;
    QVBoxLayout *verticalLayout_3;
    QWidget *widget_3;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QTextBrowser *textBrowser;
    QWidget *widget_8;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *btn_clean;
    QTextEdit *textEdit;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btn_exit;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btn_send;
    QWidget *widget_6;
    QHBoxLayout *horizontalLayout_3;
    QTableWidget *tableWidget;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(772, 550);
        horizontalLayout_6 = new QHBoxLayout(Widget);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        widget_7 = new QWidget(Widget);
        widget_7->setObjectName("widget_7");
        horizontalLayout_4 = new QHBoxLayout(widget_7);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        widget_5 = new QWidget(widget_7);
        widget_5->setObjectName("widget_5");
        verticalLayout_3 = new QVBoxLayout(widget_5);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        widget_3 = new QWidget(widget_5);
        widget_3->setObjectName("widget_3");
        verticalLayout_2 = new QVBoxLayout(widget_3);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(widget_3);
        frame->setObjectName("frame");
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        textBrowser = new QTextBrowser(frame);
        textBrowser->setObjectName("textBrowser");
        textBrowser->setFrameShape(QFrame::Shape::Box);

        verticalLayout->addWidget(textBrowser);

        widget_8 = new QWidget(frame);
        widget_8->setObjectName("widget_8");
        horizontalLayout_5 = new QHBoxLayout(widget_8);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);

        btn_clean = new QPushButton(widget_8);
        btn_clean->setObjectName("btn_clean");
        btn_clean->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_5->addWidget(btn_clean);


        verticalLayout->addWidget(widget_8);

        textEdit = new QTextEdit(frame);
        textEdit->setObjectName("textEdit");
        textEdit->setMaximumSize(QSize(16777215, 100));
        textEdit->setFrameShape(QFrame::Shape::Box);

        verticalLayout->addWidget(textEdit);


        verticalLayout_2->addWidget(frame);

        widget_2 = new QWidget(widget_3);
        widget_2->setObjectName("widget_2");
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);

        verticalLayout_2->addWidget(widget_2);


        verticalLayout_3->addWidget(widget_3);

        widget_4 = new QWidget(widget_5);
        widget_4->setObjectName("widget_4");
        horizontalLayout_2 = new QHBoxLayout(widget_4);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        btn_exit = new QPushButton(widget_4);
        btn_exit->setObjectName("btn_exit");

        horizontalLayout_2->addWidget(btn_exit);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        label = new QLabel(widget_4);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        btn_send = new QPushButton(widget_4);
        btn_send->setObjectName("btn_send");

        horizontalLayout_2->addWidget(btn_send);


        verticalLayout_3->addWidget(widget_4);


        horizontalLayout_4->addWidget(widget_5);

        widget_6 = new QWidget(widget_7);
        widget_6->setObjectName("widget_6");
        widget_6->setMaximumSize(QSize(200, 16777215));
        horizontalLayout_3 = new QHBoxLayout(widget_6);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        tableWidget = new QTableWidget(widget_6);
        tableWidget->setObjectName("tableWidget");
        tableWidget->setFrameShape(QFrame::Shape::Box);

        horizontalLayout_3->addWidget(tableWidget);


        horizontalLayout_4->addWidget(widget_6);


        horizontalLayout_6->addWidget(widget_7);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        btn_clean->setText(QCoreApplication::translate("Widget", "Clean", nullptr));
        btn_exit->setText(QCoreApplication::translate("Widget", "Exit", nullptr));
        label->setText(QCoreApplication::translate("Widget", "Oline\357\274\2320", nullptr));
        btn_send->setText(QCoreApplication::translate("Widget", "Send", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
