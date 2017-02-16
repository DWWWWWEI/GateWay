#include "devicecontrollercom.h"
#include <QDebug>
DeviceControllerCom::DeviceControllerCom(QObject *parent) : QObject(parent)
{
    QString protocol=QString("modbus");
    ProtocolHandlerFactory factory;
    protocolHandler=factory.getProtocolHandlerInstance(protocol);
    isReadWrite=true;
    connect(protocolHandler,SIGNAL(readFinished(quint64,quint32,QByteArray)),this,SLOT(handleReadFinished(quint64,quint32,QByteArray)));
    connect(protocolHandler,SIGNAL(writeFinished(QString,QString)),this,SLOT(handleWriteFinished(QString,QString)));
}
bool DeviceControllerCom::read(const QByteArray &controllerAddress,const QByteArray &funcDeviceAddress)
{
    if(isReadWrite){
        ctrlAddr=controllerAddress;
        funcAddr=funcDeviceAddress;
        protocolHandler->read(ctrlAddr,funcAddr,RS485);
        readTimer=startTimer(3000);
        isReadWrite=false;
        return true;
    }
    return false;
}
bool DeviceControllerCom::write(const QByteArray &controllerAddress,const QByteArray &funcDeviceAddress,
           const QByteArray &sourceData)
{
    if(isReadWrite){
        ctrlAddr=controllerAddress;
        funcAddr=funcDeviceAddress;
        bool ok;
        QString strCtrlAddr=QString::number(ctrlAddr.toHex().toUInt(&ok,16));
        if(ok==false)
            return false;
        QString strFuncAddr=QString::number(funcAddr.toHex().toUInt(&ok,16));
        if(ok==false)
            return false;
        qDebug()<<strCtrlAddr<<" "<<strFuncAddr<<" "<<sourceData;
        protocolHandler->write(strCtrlAddr,strFuncAddr,RS485,sourceData);
        writeTimer=startTimer(3000);
        return true;
    }
    return false;
}
void DeviceControllerCom::handleReadFinished(quint64 controllerAddress, quint32 funcDeviceAddress, const QByteArray &data){
    isReadWrite=true;
    killTimer(readTimer);
    QByteArray controllerAddr;
    controllerAddr.append(controllerAddress<<56);
    controllerAddr.append(controllerAddress<<48);
    controllerAddr.append(controllerAddress<<40);
    controllerAddr.append(controllerAddress<<32);
    controllerAddr.append(controllerAddress<<24);
    controllerAddr.append(controllerAddress<<16);
    controllerAddr.append(controllerAddress<<8);
    controllerAddr.append(controllerAddress);
    QByteArray funcDeviceAddr;
    funcDeviceAddr.append(funcDeviceAddress<<24);
    funcDeviceAddr.append(funcDeviceAddress<<16);
    funcDeviceAddr.append(funcDeviceAddress<<8);
    funcDeviceAddr.append(funcDeviceAddress);
    emit readFinished(controllerAddr,funcDeviceAddr,data,true);
}
void DeviceControllerCom::handleWriteFinished(const QString &controllerAddress, const QString &funcDeviceAddress){
    isReadWrite=true;
    killTimer(writeTimer);
    emit writeFinished(controllerAddress.toLatin1(),funcDeviceAddress.toLatin1(),true);
}
void DeviceControllerCom::timerEvent(QTimerEvent *event){
    isReadWrite=true;
    bool ok;
    if(event->timerId()==writeTimer){
        emit writeFinished(ctrlAddr,funcAddr,false);
    }else if(event->timerId()==readTimer){
        emit readFinished(ctrlAddr,funcAddr,readData,false);
    }
}
