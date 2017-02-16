#include "modbushandler.h"
#include <QDebug>
ModbusHandler::ModbusHandler():modbusDevice(Q_NULLPTR)
{
    if(modbusDevice){
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice=Q_NULLPTR;
    }
    modbusDevice=new QModbusRtuSerialMaster();
    if(!modbusDevice){
        qDebug()<<"Could not create Modbus Master";
    }
    connectModbusDevice();
}
ModbusHandler::~ModbusHandler(){
    if(modbusDevice)
        modbusDevice->disconnectDevice();
    delete modbusDevice;
}

//根据参数communicationType（承载方式）将服务器发送的读请求发送给不同的通信承载程序处理，异步

bool ModbusHandler::read(const QByteArray& controllerAddress,const QByteArray& funcDeviceAddress,
                  const ControllerCommCarrierType communicationType)
{
    bool ok;
    int intCtrlAddress=controllerAddress.right(4).toHex().toInt(&ok);
    if(ok==false){
        return false;
    }
    int intFuncAddress=funcDeviceAddress.toHex().toInt(&ok);
    if(ok==false){
        return false;
    }
    read(intCtrlAddress,intFuncAddress,communicationType);
    return false;
}
bool ModbusHandler::read(const quint64 controllerAddress,const quint32 funcDeviceAddress,
                      const ControllerCommCarrierType communicationType)
{
    if(!modbusDevice){
        return false;
    }

    //寄存器个数
    quint16 count=1;
    //设置读取的寄存器类型，寄存器地址，寄存器个数
    QModbusDataUnit readUnit=QModbusDataUnit(QModbusDataUnit::HoldingRegisters,funcDeviceAddress,count);
    if(reply=modbusDevice->sendReadRequest(readUnit,controllerAddress)){
        if(!reply->isFinished()){
            readControllerAddr=controllerAddress;
            readFuncDeviceAddr=funcDeviceAddress;
            connect(reply,&QModbusReply::finished,this,&ModbusHandler::readReady);
            return true;
        }else{
            delete reply;
            return false;
        }
    }else{
        return false;
    }

}

bool ModbusHandler::read(const QString &controllerAddress,const QString &funcDeviceAddress,
                         const ControllerCommCarrierType communicationType)
{
    QByteArray serverAddress=controllerAddress.toLatin1().right(4);
    bool ok;
    int intFuncDeviceAdrs = funcDeviceAddress.toInt(&ok);
    if(ok==false)
        return false;
    int intServerAddress=serverAddress.toInt(&ok);
    if(ok==false)
        return false;
    return read(intServerAddress,intFuncDeviceAdrs,communicationType);
}
void ModbusHandler::readReady(){
    if(!reply)
        return;
    if(reply->error()==QModbusDevice::NoError){
        const QModbusDataUnit unit=reply->result();
        float value=float(unit.value(0))/10;
        QString data=QString::number(value,'f',1);
        qDebug()<<data.toLatin1();
        emit readFinished(readControllerAddr,readFuncDeviceAddr,data.toLatin1());
        //emit readFinished(QString::number(readControllerAddr, 16).toUpper(),
        //                  QString::number(readFuncDeviceAddr, 16).toUpper(),data.toLatin1());
    }else{
        qDebug()<<"modbus read error:"<<reply->errorString();
        //这里出现了错误， 因此不应该再emit readFinished信号
        //emit readFinished(readControllerAddr,readFuncDeviceAddr,QByteArray());
    }
}
//将服务器发送的写请求发送给不同的通讯承载程序,异步
bool ModbusHandler::write(const QString &controllerAddress,const QString &funcDeviceAddress,
                   const ControllerCommCarrierType communicationType,const QByteArray &sourceData)
{
    if(!modbusDevice){
            return false;
        }
        quint16 regCount;
        QByteArray writeData=sourceData;
        if(0==writeData.size()%2){
            regCount=writeData.size()/2;
        }else{
            regCount=(writeData.size()/2)+1;
        }
        bool ok;
        QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,funcDeviceAddress.toInt(&ok),regCount);
        if(ok==false)
            return false;
        QByteArray serverAddress=controllerAddress.toLatin1().right(4);
        for(uint i=0;i<writeUnit.valueCount();i++){
            QByteArray data;
            data.append(writeData[2*i]);
            data.append(writeData[2*i+1]);
            writeUnit.setValue(i,data.toHex().toShort(nullptr,16));
        }
        int intServerAddress=serverAddress.toInt(&ok);
        if(ok==false)
            return false;
        if(reply=modbusDevice->sendWriteRequest(writeUnit,intServerAddress)){
            if(!reply->isFinished()){
                writeControllerAddr=controllerAddress;
                writeFuncDeviceAddr=funcDeviceAddress;
                connect(reply,&QModbusReply::finished,this,&ModbusHandler::writeReady);
                return true;
            }else{
                delete reply;
                return false;
            }
        }else{
            return false;
        }
}
void ModbusHandler::writeReady(){
    if(!reply)
        return;
    if(reply->error()==QModbusDevice::NoError){
        emit writeFinished(writeControllerAddr,writeFuncDeviceAddr);
    }else{
        qDebug()<<"modbus write error:"<<reply->errorString();
    }
}
void ModbusHandler::connectModbusDevice(){
    if(!modbusDevice)
        return;
    //设置参数如波特率、校验位、停止位、数据位
    QString portName="COM5";
    if(modbusDevice->state()!=QModbusDevice::ConnectedState){
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,portName);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud9600);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
        modbusDevice->setTimeout(3000);
        modbusDevice->setNumberOfRetries(0);
        bool connect=modbusDevice->connectDevice();
        if(connect==true){
            qDebug()<<"connect success";
        }else{
            qDebug()<<"connect failed";
            //尝试关闭
            modbusDevice->disconnectDevice();
        }
    }
}

ProtocolType ModbusHandler::getProtocol()
{
    return Modbus;
}
