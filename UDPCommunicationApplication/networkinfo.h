#ifndef NETWORKINFO_H
#define NETWORKINFO_H

#include <QDialog>
#include <QNetworkInterface>

namespace Ui {
class networkInfo;
}

class networkInfo : public QDialog
{
    Q_OBJECT

public:
    explicit networkInfo(QWidget *parent = nullptr);
    ~networkInfo();

private:
    Ui::networkInfo *ui;
};

#endif // NETWORKINFO_H
