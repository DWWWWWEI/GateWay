#include "protocolhandlerfactory.h"

ProtocolHandlerFactory::ProtocolHandlerFactory()
{
}
IProtocolHandler* ProtocolHandlerFactory::getProtocolHandlerInstance(QString &protocol){
    if("modbus"==protocol.toLower()){
        return Singleton< ModbusHandler>::instance();
    }
    return Q_NULLPTR;
}
