#include <QCoreApplication>
#include "gwengine.h"
#include <QDebug>
#include <QThread>
#include "protocolhandlerfactory.h"
#include "iprotocolhandler.h"
#include "test.h"
#include "devicemanager.h"
#include "connectionmanager.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    GWEngine * engine = Singleton<GWEngine>::instance();
    engine->start();

//    ConnectionManager *manager=Singleton<ConnectionManager>::instance();
//    QString ip="127.0.0.1";
//    int port=4000;
//    manager->start(ip,port);

//    DeviceManager * device = Singleton<DeviceManager>::instance();
//    QByteArray addr;
//    QByteArray data("{\"dataContent\":{\"controllers\":[{\"devices\":[{\"accessType\":\"RW\",\"funcAddress\":1,\"funcName\":\"temp1\"}],\"name\":\"nnnnn1\"}],\"operation\":\"set\"},\"dataType\":\"Object\"}");
//    device->handleWriteMsg(addr,data);
    /**
     * servercommhelper测试
     */
//    Test *test=new Test();


    /**
     * modbushandler读测试
     * 连接串口在modbushandler的connectModbusDevice()中定义
     */
//    ProtocolHandlerFactory* factory=new ProtocolHandlerFactory();
//    QString protocol="modbus";
//    IProtocolHandler* modbus=factory->getProtocolHandlerInstance(protocol);
//    int controlAddress=1;
//    int functionAddress=1;
//    ControllerCommCarrierType communicationType=ControllerCommCarrierType::RS485;
//    modbus->read(QString::number(controlAddress),QString::number(functionAddress),communicationType);

    /**
      modbushandler写测试(未进行硬件测试)
     */
//    ProtocolHandlerFactory* factory=new ProtocolHandlerFactory();
//    QString protocol="modbus";
//    IProtocolHandler* modbus=factory->getProtocolHandlerInstance(protocol);
//    int controlAddress=1;
//    int functionAddress=2;
//    ControllerCommCarrierType communicationType=ControllerCommCarrierType::RS485;
//    QByteArray sourceData;
//    short data=22;
//    sourceData.append(data>>8);
//    sourceData.append(data);
//    modbus->write(QString::number(controlAddress),QString::number(functionAddress),communicationType,sourceData);
    return a.exec();
}
