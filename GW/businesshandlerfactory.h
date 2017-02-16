#ifndef BUSINESSHANDLERFACTORY_H
#define BUSINESSHANDLERFACTORY_H
#include <QList>
#include "ibusinesshandler.h"
#include "singleton.h"
#include "devicemanager.h"
#include "watersupplyhandler.h"
class BusinessHandlerFactory
{
private:
    BusinessHandlerFactory();
public:
    static IBusinessHandler * getBusinessHandler(BussinessType type)
    {
        if (type==DeviceManagement)
            return Singleton<DeviceManager>::instance();
        else if(type==WaterSupply2nd)
            return Singleton<WaterSupplyHandler>::instance();
        else
            return NULL;
    }
    static QList<IBusinessHandler *> getAllBizHandler()
    {
        QList<IBusinessHandler *> bhList;
        bhList.append(Singleton<DeviceManager>::instance());
        bhList.append(Singleton<WaterSupplyHandler>::instance());
        //TODO:
        //使用插件矿建动态查询并加载并获得所有业务处理插件的handler
        return bhList;
    }
};

#endif // BUSINESSHANDLERFACTORY_H
