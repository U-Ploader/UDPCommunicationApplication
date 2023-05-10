
#ifndef WIDGET_H
#define WIDGET_H

#include <networkinfo.h>

#include <QWidget>
#include <QMessageBox>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QRegularExpressionValidator>
#include <QFile>
#include <QFileDialog>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget

{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    //声明槽函数
    void sendData();//发送数据
    void receiveData();//接收数据
    void sendStart();//开始发送
    void receiveStart();//开始监听
    void sendStop();//停止发送
    void timedSending_stateChanged();//定时发送
    void send_clear();//发送区清空
    void receive_clear();//接收区清空
    void count_clear();//计数清零
    void networkConfig();//网络信息
    void saveData();//保存数据

private:
    Ui::Widget *ui;

    networkInfo *networkinfo;

    //声明指针对象
    QUdpSocket *sender;
    QUdpSocket *receiver;
    QTimer *timer;

    //声明函数
    bool ipAddrIsOK(const QString & ip);
};

#endif // WIDGET_H
