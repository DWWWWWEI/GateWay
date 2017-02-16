#ifndef DEVICECONTROLLERCOM_H
#define DEVICECONTROLLERCOM_H

#include <QObject>
#include <QTimerEvent>
#include "singleton.h"
#include "iprotocolhandler.h"
#include "modbushandler.h"
#include "protocolhandlerfactory.h"
/************************************************************************
 *
**该类的职责:屏蔽了各设备间协议和物理承载的差异;
**根据控制器地址查询该控制器的通讯协议和承载方式,
** 并调用相应的协议处理类进行具体发送（读/写）操作
**************************************************************************/
class DeviceControllerCom : public QObject
{
    Q_OBJECT
private:
    explicit DeviceControllerCom(QObject *parent = 0);
//public slots:
public:
    //static DeviceControllerCom * getInstance();
    //发送服务器传输的读取设备数据请求，并根据物理地址的特定协议方式发送给不同的协议处理。
    bool read(const QByteArray &controllerAddress,const QByteArray &funcDeviceAddress);
    //发送服务器传输的写入设备数据请求
    bool write(const QByteArray &controllerAddress,const QByteArray &funcDeviceAddress,
               const QByteArray &sourceData);
signals:
    void readyToWrite(const QByteArray &controllerAddress,const QByteArray &funcDeviceAddress,QByteArray &source);
    void readyToRead(const QByteArray &controllerAddress,const QByteArray &funcDeviceAddress);

    void writeFinished(const QByteArray &controllerAddress,const QByteArray &funcDeviceAddress,
                  bool operationResult);
    void readFinished(QByteArray controllerAddress,QByteArray funcDeviceAddress,
                 const QByteArray &data,bool operationResult);
    friend class Singleton<DeviceControllerCom>;
private:
    IProtocolHandler * protocolHandler;
    int writeTimer;
    int readTimer;
    QByteArray ctrlAddr;
    QByteArray funcAddr;
    QByteArray readData;
    bool isReadWrite;
private slots:
    void timerEvent(QTimerEvent *event);
public slots:
    void handleReadFinished(quint64 controllerAddress,quint32 funcDeviceAddress,const QByteArray &data);
    void handleWriteFinished(const  QString& controllerAddress,const QString &funcDeviceAddress);
};

#endif // DEVICECONTROLLERCOM_H
