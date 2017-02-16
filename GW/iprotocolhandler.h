#ifndef IPROTOCOLHANDLER_H
#define IPROTOCOLHANDLER_H

#include <QObject>
#include "commontypesdef.h"
class IProtocolHandler : public QObject
{
    Q_OBJECT
public:
    explicit IProtocolHandler(QObject *parent = 0);

    //根据参数communicationType（承载方式）将服务器发送的读请求发送给不同的通信承载程序处理
    virtual bool read(const QByteArray& controllerAddress,const QByteArray& funcDeviceAddress,
                      const ControllerCommCarrierType communicationType)=0;
    virtual bool read(const QString &controllerAddress,const QString &funcDeviceAddress,
                      const ControllerCommCarrierType communicationType)=0;
    virtual bool read(const quint64 controllerAddress,const quint32 funcDeviceAddress,
                      const ControllerCommCarrierType communicationType)=0;
    //将服务器发送的写请求发送给不同的通讯承载程序
    virtual bool write(const QString &controllerAddress,const QString &funcDeviceAddress,
                       const ControllerCommCarrierType communicationType,const QByteArray &sourceData)=0;
    virtual ProtocolType getProtocol()=0;

    virtual ~IProtocolHandler(){}
    signals:
    //void readFinished(const  QString& controllerAddress,const QString &funcDeviceAddress,const QByteArray &data);
    void readFinished(const  quint64 controllerAddress,quint32 funcDeviceAddress,const QByteArray &data);
    void writeFinished(const  QString& controllerAddress,const QString &funcDeviceAddress);
signals:

public slots:
};

#endif // IPROTOCOLHANDLER_H
