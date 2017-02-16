#ifndef RS485CARRIER_H
#define RS485CARRIER_H
#include "icommunicationcarrier.h"

class RS485Carrier : public ICommunicationCarrier
{
public:
    RS485Carrier();

    virtual bool init();

    //发送数据给特定的设备, 要求各子类以异步方式实现
    virtual bool send(const  QString& deviceAddress,const QByteArray &data);
    virtual bool isReady();
    virtual ControllerCommCarrierType getComType();
};

#endif // RS485CARRIER_H
