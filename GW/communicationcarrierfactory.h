#ifndef COMMUNICATIONCARRIERFACTORY_H
#define COMMUNICATIONCARRIERFACTORY_H
#include "icommunicationcarrier.h"
#include "singleton.h"
#include "rs485carrier.h"
class CommunicationCarrierFactory
{
private:
    CommunicationCarrierFactory();
public:
    static ICommunicationCarrier * getCommunicationCarrier(ControllerCommCarrierType type)
    {
        if (type==RS485)
            return Singleton<RS485Carrier>::instance();
        else
            return NULL;
    }
};

#endif // COMMUNICATIONCARRIERFACTORY_H
