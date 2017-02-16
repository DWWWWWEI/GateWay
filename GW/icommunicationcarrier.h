#ifndef ICOMMUNICATIONCARRIER_H
#define ICOMMUNICATIONCARRIER_H

#include <QObject>
#include "commontypesdef.h"
class ICommunicationCarrier : public QObject
{
    Q_OBJECT
public:
    explicit ICommunicationCarrier(QObject *parent = 0);
    virtual bool init()=0;

    //发送数据给特定的设备, 要求各子类以异步方式实现
    virtual bool send(const  QString& deviceAddress,const QByteArray &data)=0;
    virtual bool isReady()=0;
    virtual ControllerCommCarrierType getComType()=0;
    virtual ~ICommunicationCarrier(){}
signals:
    void dataReceived(const QString& deviceAddress,const QByteArray& data);
public slots:
};

#endif // ICOMMUNICATIONCARRIER_H
