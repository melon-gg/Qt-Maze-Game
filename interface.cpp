#include "interface.h"
#include <QPalette>
#include <QPixmap>
#include <QFont>
#include <QDebug>

interface::interface(QWidget *parent) :
    QWidget(parent)
{
    msgLabel = new QLabel(this);//提示信息
    msgLabel->setGeometry(380,440,100,75);
    msgLabel->setStyleSheet("color:white");
    msgLabel->setFont(QFont("Microsoft YaHei",13));
    msgLabel->hide();


    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(loading()));

    QFont font("Microsoft YaHei" ,16, 55);
    QString button_style="QPushButton{background-color:transparent;color:white;border-radius:5px;}"
                         "QPushButton:hover{background-color:palegreen; color: orangered;}"
                         "QPushButton:pressed{background-color:aquamarine;border-style:inset;}";

    isok=false;
    classicalisok=false;

	classicalStart();
}

void interface::paintEvent(QPaintEvent*)
{
    painter = new QPainter(this);
    if(!isok)
    {
        QImage pixmap(":/new/image/StartPage.png");//开始页
        painter->drawImage(geometry(),pixmap);
    }
    painter->end();
}

void interface::classicalStart()
{
    msgLabel->show();//提示消息显示
    classicalisok=true;
}

void interface::Quit()
{
    QMessageBox message(QMessageBox::Information,"退出游戏 ","是否退出? ",QMessageBox::Yes|QMessageBox::No,this);
    message.setIconPixmap(QPixmap(":/info/image/information/退出.png"));
    message.setWindowIcon(QIcon(":/info/image/information/关于.ico"));
    message.setButtonText(QMessageBox::Yes, QString("确 定 "));
    message.setButtonText(QMessageBox::No, QString("返 回 "));
    if(message.exec()==QMessageBox::Yes)
    {
        qApp->quit();//退出当前应用程序，不需要 QApplication的头文件
    }
    else
    {
        return;
    }
}
