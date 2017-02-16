#ifndef MODBUSHANDLER_H
#define MODBUSHANDLER_H
#include "iprotocolhandler.h"
#include "singleton.h"
#include "commontypesdef.h"
#include <QtSerialBus/QModbusRtuSerialMaster>
#include <QtSerialBus/QModbusDataUnit>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QList>
class ModbusHandler : public IProtocolHandler
{

private:
    ModbusHandler();
public:
    ~ModbusHandler();


    //根据参数communicationType（承载方式）将服务器发送的读请求发送给不同的通信承载程序处理
    virtual bool read(const QString &controllerAddress,const QString &funcDeviceAddress,
                      const ControllerCommCarrierType communicationType);

    virtual bool read(const QByteArray& controllerAddress,const QByteArray& funcDeviceAddress,
                      const ControllerCommCarrierType communicationType);
    virtual bool read(const quint64 controllerAddress,const quint32 funcDeviceAddress,
                      const ControllerCommCarrierType communicationType);
    //将服务器发送的写请求发送给不同的通讯承载程序
    /*
     * QString->ControllerCommCarrierType
     *
     */
    virtual bool write(const QString &controllerAddress,const QString &funcDeviceAddress,
                       const ControllerCommCarrierType communicationType,const QByteArray &sourceData);
    virtual ProtocolType getProtocol();
    friend class Singleton<ModbusHandler>;
private:
    QModbusReply *reply;
    QModbusClient *modbusDevice;
    QList<QString> writeList;
    QList<QString> readList;
    quint64 readControllerAddr;
    quint32 readFuncDeviceAddr;
    QString writeControllerAddr;
    QString writeFuncDeviceAddr;
    void connectModbusDevice();
public slots:
    void readReady();
    void writeReady();
};

#endif // MODBUSHANDLER_H
