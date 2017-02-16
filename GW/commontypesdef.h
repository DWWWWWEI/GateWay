#ifndef COMMONENUMDEF_H
#define COMMONENUMDEF_H
#include <qstring.h>
enum BussinessType{
    ServerConnection=0,
    DeviceManagement=1,
    WaterSupply2nd=2,
    UnknownBussinessType=100,
};

enum ProtocolType{
    Modbus,
    UnknownProtocolType,
};

enum ControllerCommCarrierType{
    RS485,
    TCPServer,
    ZigBee,
    PowerLine,
    UnknownCtrlCommCarrierType,
};
enum OperationType{
    Read,
    Write,
};

class BussinessTypeUtil{
public:
    static const QString& getTypeString(BussinessType type){
        switch (type) {
        case DeviceManagement:
            return QString("DeviceManagement");
        case WaterSupply2nd:
            return QString("WaterSupply2nd");
        default:
            return QString("UnknownBussinessType");
        }
    }
    static const BussinessType getTypeEnum(const QString &type){
        if(type=="DeviceManagement")
            return DeviceManagement;
        if(type=="WaterSupply2nd")
            return WaterSupply2nd;
        return UnknownBussinessType;
    }
};


class ProtocolTypeUtil{
public:
    static const QString& getTypeString(ProtocolType type){
        switch (type) {
        case Modbus:
            return QString("Modbus");

        default:
            return QString("UnknownProtocolType");
        }
    }
    static const ProtocolType getTypeEnum(const QString &type){
        if(type=="Modbus")
            return Modbus;
        return UnknownProtocolType;
    }
};

class CtrlCommCarrierTypeUtil{
public:
    static const QString& getTypeString(ControllerCommCarrierType type){
        switch (type) {
        case RS485:
            return QString("RS485");
        case TCPServer:
            return QString("TCPServer");
        default:
            return QString("UnknownCtrlCommCarrierType");
        }
    }
    static const ControllerCommCarrierType getTypeEnum(const QString &type){
        if(type=="RS485")
            return RS485;
        else if(type=="TCPServer")
            return TCPServer;

        return UnknownCtrlCommCarrierType;
    }
};

#endif // COMMONENUMDEF_H
