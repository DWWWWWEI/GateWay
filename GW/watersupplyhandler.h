#ifndef WATERSUPPLYHANDLER_H
#define WATERSUPPLYHANDLER_H
#include "ibusinesshandler.h"
#include <singleton.h>
#include "modbushandler.h"
#include "devicecontrollercom.h"
#include <QTimer>
#include <QJsonArray>
#include "devicemanager.h"
class WaterSupplyHandler : public QObject, public IBusinessHandler
{
    Q_OBJECT
private:
    explicit WaterSupplyHandler(QObject *parent = 0);
public:
    bool init();
    virtual bool handleWriteMsg(const  QByteArray &targetAddress,const  QByteArray &data){return true;}
    virtual bool handleReadMsg(const  QByteArray &targetAddress, QByteArray & replyData){return true;}

    /*返回为true表示通过筛选， GWEngine将继续处理该写消息；否则终止处理，使用returnValue的值作为返回给服务器端的值*/
    virtual bool filterWriteMsg(const  QByteArray &source, bool &returnValue){return true;}

    /*返回为true表示通过筛选， GWEngine将继续处理该写消息；否则终止处理，使用returnValue的值作为返回给服务器端的值*/
    virtual bool filterReadMsg(const  QByteArray &source, bool &returnValue,QByteArray & replyData)
    {return true;}
    virtual BussinessType getBusinessType(){return WaterSupply2nd;}
    friend class Singleton<WaterSupplyHandler>;
    virtual ~WaterSupplyHandler(){}
private:
    bool readDeviceData();
  //  bool getDeviceConfig(QJsonArray& controllerList);
    bool create1stConfigFile();
    bool createConfigFile(QJsonDocument &doc);


    bool deviceExist(quint32 cntrIndex, quint32 deviceIndex);
    bool readDevice(quint32 cntrIndex, quint32 deviceIndex);
    bool hasNextDevice();
    bool readNextDevice();
    bool readNextDeviceOrReport();

    QJsonArray cotrollers;
    quint32 currentCtrlIndex;
    //quint64 currentFuncAddrs;

    quint32 currentDeviceIndex;
    //quint32 currentCtrlAddrs;

    QJsonArray currentCtrlArray;
    QJsonArray currentDeviceArray;

    QJsonArray data2Report;
    int pollingInterval=10*1000;
    int readTimeoutInterval=3*1000;
    QTimer * readTimeout;
    QTimer * startPolling;
    //ModbusHandler * modbus;
    DeviceControllerCom * devCtrlCom;
    DeviceManager * devManager;
public slots:
    void startPollingDevices();
    void readDevicesTimeOut();
    //void handleReadFinished(quint64 controllerAddress,quint32 funcDeviceAddress,const QByteArray &data);
    void handleReadFinished(QByteArray controllerAddress,QByteArray funcDeviceAddress,const QByteArray &data,bool opResult);
    void handleDeviceConfigReceived();

};

#endif
