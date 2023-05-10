
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //设置窗口标题和大小
    setWindowTitle("UDPCommunicationApplication");
    setFixedSize(640,380);
    //关闭间隔时间输入框和停止按钮
    ui->lineEdit_send_time->setEnabled(false);
    ui->pBtn_send_stop->setEnabled(false);
    //创建输入规则
    QRegularExpression rx1("[0-9]+$");
    QRegularExpression rx2("[0-9\\.]+$");
    ui->lineEdit_destnPort->setValidator(new QRegularExpressionValidator(rx1, this));
    ui->lineEdit_listenPort->setValidator(new QRegularExpressionValidator(rx1, this));
    ui->lineEdit_destnIP->setValidator(new QRegularExpressionValidator(rx2, this));
    ui->lineEdit_send_time->setValidator(new QRegularExpressionValidator(rx1, this));
    //设置接收区和计数文本框不可取得焦点
    ui->textBrowser_receive->setFocusPolicy(Qt::NoFocus);
    ui->lineEdit_sendCount->setFocusPolicy(Qt::NoFocus);
    ui->lineEdit_receiveCount->setFocusPolicy(Qt::NoFocus);
    //创建socket对象
    sender = new QUdpSocket(this);
    receiver = new QUdpSocket(this);
    //创建定时器对象
    timer = new QTimer(this);
    //连接信号和槽
    QObject::connect(ui->pBtn_send,SIGNAL(clicked()),this,SLOT(sendStart()));//连接发送按钮和开始发送函数
    QObject::connect(ui->lineEdit_send,SIGNAL(returnPressed()),this,SLOT(sendStart()));//连接回车和开始发送函数
    QObject::connect(ui->pBtn_listenStart,SIGNAL(clicked()),this,SLOT(receiveStart()));//连接监听按钮和开始监听函数
    QObject::connect(ui->checkBox_timedSending,SIGNAL(clicked()),this,SLOT(timedSending_stateChanged()));//连接定时发送复选框和timedSending_stateChanged()
    QObject::connect(ui->pBtn_send_stop,SIGNAL(clicked()),this,SLOT(sendStop()));//连接停止按钮和停止发送函数
    QObject::connect(ui->pBtn_send_clear,SIGNAL(clicked()),this,SLOT(send_clear()));//连接发送区清空按钮和发送区清空函数
    QObject::connect(ui->pBtn_receive_clear,SIGNAL(clicked()),this,SLOT(receive_clear()));//连接接收区清空按钮和接收区清空函数
    QObject::connect(ui->pBtn_count_clear,SIGNAL(clicked()),this,SLOT(count_clear()));//连接计数清零按钮和计数清零函数
    QObject::connect(ui->pBtn_networkConfig,SIGNAL(clicked()),this,SLOT(networkConfig()));//连接本机网络信息按钮和显示网络信息函数
    QObject::connect(ui->pBtn_saveData,SIGNAL(clicked()),this,SLOT(saveData()));//连接保存数据按钮和保存数据函数
    QObject::connect(receiver,&QUdpSocket::readyRead,this,&Widget::receiveData);//连接套接字接收信号和数据接收函数
    QObject::connect(timer,&QTimer::timeout,this,&Widget::sendData);//连接计时信号和数据发送函数
}

Widget::~Widget()
{
    delete ui;
}

//数据发送函数
void Widget::sendData()
{
    //获取目的ip地址和端口号
    QString destnIP = ui->lineEdit_destnIP->text();
    QString destnPort = ui->lineEdit_destnPort->text();
    //获取发送的数据
    QByteArray arr_send = ui->lineEdit_send->text().toUtf8();
    //发送数据
    sender->writeDatagram(arr_send,QHostAddress(destnIP),destnPort.toUShort());
    //发送计数
    ui->lineEdit_sendCount->setText(tr("%1").arg(arr_send.size() + ui->lineEdit_sendCount->text().toInt()));
}

//数据接收函数
void Widget::receiveData()
{
    //创建数据缓冲区
    QByteArray arr_receive;
    //创建发送端的ip
    QHostAddress senderIP;

    while(receiver->hasPendingDatagrams())
    {
        //调整缓冲区大小和收到的数据一致
        arr_receive.resize(receiver->pendingDatagramSize());
        //接收数据
        receiver->readDatagram(arr_receive.data(),arr_receive.size(),&senderIP,nullptr);
        //移动光标到最后一行
        ui->textBrowser_receive->moveCursor(QTextCursor::End);
        //显示数据
        ui->textBrowser_receive->insertPlainText("时间:"+QDateTime::currentDateTime().time().toString()+"  IP地址:"+QHostAddress(senderIP.toIPv4Address()).toString()+"  内容:"+QString(arr_receive)+"\n");
        //接收计数
        ui->lineEdit_receiveCount->setText(tr("%1").arg(arr_receive.size() + ui->lineEdit_receiveCount->text().toInt()));
    }
}

//开始发送函数
void Widget::sendStart()
{
    //如果目标IP地址不合法，弹出警告
    if(!ipAddrIsOK(ui->lineEdit_destnIP->text()))
        QMessageBox::critical(this,"警告","目标IP地址不合法");
    else
    {
        //如果目标端口不合法，弹出警告
        if(ui->lineEdit_destnPort->text().toUInt() > 65535 || ui->lineEdit_destnPort->text() == "")
            QMessageBox::critical(this,"警告","目标端口必须在 0~65535 之间");
        else
        {
            //如果没有勾选定时发送，只执行一次数据发送函数
            if(ui->checkBox_timedSending->checkState() == Qt::Unchecked)
                sendData();
            //如果勾选定时发送，利用QTimer定时触发数据发送函数
            if(ui->checkBox_timedSending->checkState() == Qt::Checked)
            {
                //如果定时发送时间为空，弹出警告
                if(ui->lineEdit_send_time->text() == "")
                    QMessageBox::critical(this,"警告","定时发送时间不能为空");
                else
                {
                    ui->lineEdit_send->setEnabled(false);
                    ui->pBtn_send->setEnabled(false);
                    ui->pBtn_send_clear->setEnabled(false);
                    ui->pBtn_send_stop->setEnabled(true);
                    ui->lineEdit_destnIP->setEnabled(false);
                    ui->lineEdit_destnPort->setEnabled(false);
                    unsigned int time = ui->lineEdit_send_time->text().toUInt();//获取计时时间
                    timer->start(time);//开始计时
                }
            }
        }
    }
}

//停止发送函数
void Widget::sendStop()
{
    ui->lineEdit_send->setEnabled(true);
    ui->pBtn_send->setEnabled(true);
    ui->pBtn_send_clear->setEnabled(true);
    ui->pBtn_send_stop->setEnabled(false);
    ui->lineEdit_destnIP->setEnabled(true);
    ui->lineEdit_destnPort->setEnabled(true);
    timer->stop();//停止计时
}

//开始接收函数
void Widget::receiveStart()
{
    if(ui->pBtn_listenStart->text() == "开始监听")
    {
        if(ui->lineEdit_listenPort->text().toUInt() > 65535 || ui->lineEdit_listenPort->text() == "")//如果监听端口不合法，弹出警告
            QMessageBox::critical(this,"警告","监听端口必须在 0~65535 之间");
        else
        {
            ui->pBtn_listenStart->setText("停止监听");
            ui->lineEdit_listenPort->setEnabled(false);
            //获取监听端口
            quint16 listenPort = ui->lineEdit_listenPort->text().toUShort();
            //绑定端口
            receiver->bind(QHostAddress::Any,listenPort);
        }
    }
    else
    {
        ui->pBtn_listenStart->setText("开始监听");
        ui->lineEdit_listenPort->setEnabled(true);
        //关闭socket连接
        receiver->close();
    }
}

//定时发送复选框状态改变联动send_time输入框开关
void Widget::timedSending_stateChanged()
{
    if(ui->checkBox_timedSending->checkState() == Qt::Unchecked)
        ui->lineEdit_send_time->setEnabled(false);
    if(ui->checkBox_timedSending->checkState() == Qt::Checked)
        ui->lineEdit_send_time->setEnabled(true);
}

//发送区清空函数
void Widget::send_clear()
{
    ui->lineEdit_send->clear();
}

//接收区清空函数
void  Widget::receive_clear()
{
    ui->textBrowser_receive->clear();
}

//计数清零函数
void Widget::count_clear()
{
    ui->lineEdit_sendCount->setText("0");
    ui->lineEdit_receiveCount->setText("0");
}

//判断IP地址是否合法
bool Widget::ipAddrIsOK(const QString & ip)
{
    if (ip.isEmpty())
    {
        return false;
    }
    QStringList list = ip.split('.');
    if (list.size() != 4)
    {
        return false;
    }
    for (const auto& num : list)
    {
        bool ok = false;
        int temp = num.toInt(&ok);
        if (!ok || temp < 0 || temp > 255)
        {
            return false;
        }
    }
    return true;
}

//显示网络信息
void Widget::networkConfig()
{
    //创建新的Dialog窗口
    networkinfo = new networkInfo(this);
    networkinfo->setWindowTitle("本机网络信息");
    networkinfo->setFixedSize(480,360);
    networkinfo->exec();
}

//保存数据
void Widget::saveData()
{
    //选择保存路径
    QString textfileName = QFileDialog::getSaveFileName(this,"",tr("data"),tr("text(*.txt)"));
    QFile file(textfileName);
    if(!file.open(QFile::ReadWrite|QFile::Text))
    {
        QMessageBox::information(this,"警告","请选择合适的路径！");
        return;
    }
    QTextStream out(&file);  //QTextStream会自动将 Unicode 编码同操作系统的编码进行转换
    out<<ui->textBrowser_receive->toPlainText();
    QMessageBox::information(this,"提示","数据已成功保存");
}
