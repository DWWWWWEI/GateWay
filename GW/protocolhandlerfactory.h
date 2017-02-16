#ifndef PROTOCOLHANDLERFACTORY_H
#define PROTOCOLHANDLERFACTORY_H
#include "iprotocolhandler.h"
#include "modbushandler.h"

class ProtocolHandlerFactory
{
public:
    ProtocolHandlerFactory();
    IProtocolHandler *getProtocolHandlerInstance(QString &protocol);
};

#endif // PROTOCOLHANDLERFACTORY_H
