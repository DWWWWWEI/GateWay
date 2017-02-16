#include "rs485carrier.h"

RS485Carrier::RS485Carrier()
{

    //TODO:
    return;
}


bool RS485Carrier::init()
{

    //TODO:
    return false;
}

bool RS485Carrier::send(const  QString& deviceAddress,const QByteArray &data)
{
    //TODO:
    return false;
}

bool RS485Carrier::isReady()
{
    //TODO:
    return false;
}

ControllerCommCarrierType RS485Carrier::getComType()
{
    return RS485;
}
