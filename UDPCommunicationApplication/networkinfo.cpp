#include "networkinfo.h"
#include "ui_networkinfo.h"

networkInfo::networkInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::networkInfo)
{
    ui->setupUi(this);

    QString networkInfo;
    foreach (QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        //获取设备名
        networkInfo.append("Device: ");
        networkInfo.append(netInterface.name());
        networkInfo.append("\n");
        //获取MAC地址
        networkInfo.append("HardwareAddress: ");
        networkInfo.append(netInterface.hardwareAddress());
        networkInfo.append("\n");
        QList entryList = netInterface.addressEntries();
        //遍历每一个IP地址(每个包含一个IP地址，一个子网掩码和一个广播地址)
        foreach(QNetworkAddressEntry entry, entryList)
        {
            //获取IP地址
            networkInfo.append("IP Address: ");
            networkInfo.append(entry.ip().toString());
            networkInfo.append("\n");
            //获取子网掩码
            networkInfo.append("Netmask: ");
            networkInfo.append(entry.netmask().toString());
            networkInfo.append("\n");
            //获取广播地址
            networkInfo.append("Broadcast: ");
            networkInfo.append(entry.broadcast().toString());
            networkInfo.append("\n");
        }
        networkInfo.append("\n");
        //显示网络信息到文本框中
        ui->textBrowser->setPlainText(networkInfo);
    }
}

networkInfo::~networkInfo()
{
    delete ui;
}
