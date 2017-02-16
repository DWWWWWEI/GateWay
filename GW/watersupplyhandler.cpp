#include "watersupplyhandler.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include "servercommhelper.h"
WaterSupplyHandler::WaterSupplyHandler(QObject *parent)//:QObject(parent)
{
    readTimeout=new QTimer();
    readTimeout->setInterval(readTimeoutInterval);
    startPolling=new QTimer();
    startPolling->setInterval(pollingInterval);
    devCtrlCom = Singleton< DeviceControllerCom>::instance();
    devManager = Singleton< DeviceManager>::instance();
    connect(devManager,SIGNAL(receiveDeviceConfig()),this,SLOT(handleDeviceConfigReceived()));

    connect(readTimeout, SIGNAL(timeout()), this, SLOT(readDevicesTimeOut()));

    connect(startPolling, SIGNAL(timeout()), this, SLOT(startPollingDevices()));
}
//bool WaterSupplyHandler::getDeviceConfig(QJsonArray& controllerList)
//{
//    QFile loadFile(QStringLiteral("DeviceConfig.json"));
//          if (!loadFile.open(QIODevice::ReadOnly)) {
//              qWarning("Couldn't open save file.");
//              return false;
//          }

//    QByteArray saveData = loadFile.readAll();

//    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
//    QJsonArray localArray =loadDoc.array();
//    controllerList = localArray;
//    return true;
//}
bool WaterSupplyHandler::createConfigFile(QJsonDocument &doc)
{
    QFile saveFile( QStringLiteral("DeviceConfig.json"));

    if (!saveFile.open(QIODevice::WriteOnly)) {
              qWarning("Couldn't open save file.");
              return false;
    }
    saveFile.write(doc.toJson());
    saveFile.close();
    return true;
}

bool WaterSupplyHandler::create1stConfigFile()
{

    QJsonObject sensor1{
        {"functionAddress", 1},
        {"functionname", "temprature"},
        {"accessType","W"}
    };
    QJsonObject sensor2{
        {"functionAddress", 2},
        {"functionname", "pressure"},
        {"accessType","RW"}
    };
    QJsonObject sensor3;
    sensor3["functionAddress"]=3;
    sensor3["functionname"]="light";
    sensor3["accessType"]="R";
    QJsonArray deviceList;
    deviceList.append(sensor1);
    deviceList.append(sensor2);
    deviceList.append(sensor3);
    QJsonArray controllerList;
    QJsonObject controller;
    controller["name"]="TempController";
    controller["type"]="数显";
    controller["protocol"]="modbus";
    controller["phyCarrier"]="485";
    controller["address"]=1;
    controller["devices"]=deviceList;
    controllerList.append(controller);

    QJsonDocument doc(controllerList);
    return createConfigFile(doc);
}

void WaterSupplyHandler::readDevicesTimeOut()
{
    readTimeout->stop();
    QJsonObject device;

    int deviceAddress=cotrollers.at(currentCtrlIndex).
             toObject()["devices"].toArray().
             at(currentDeviceIndex).toObject()["functionAddress"].toInt();

    device["functionAddress"]=deviceAddress;
    device["functionValue"]=QJsonValue();
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss");
    device["timestamp"]=str;
    currentDeviceArray.append(device);

    readNextDeviceOrReport();
}

//WaterSupplyHandler::startReadDevice()
//{

//}
bool WaterSupplyHandler::deviceExist(quint32 cntrIndex, quint32 deviceIndex)
{
    qDebug()<<cotrollers;
    int deviceNum =cotrollers.at(cntrIndex).toObject()["devices"].toArray().size();
    qDebug()<<"WaterSupplyHandler::deviceExist  "<<deviceNum;
//    i = cotrollers.size();
//    QJsonArray jarray = cotrollers.at(cntrIndex).toObject()["devices"].toArray();
    if(cntrIndex>=cotrollers.size()||deviceIndex>=deviceNum)
        return false;
    return true;
}
bool WaterSupplyHandler::readDevice(quint32 cntrIndex, quint32 deviceIndex)
{
    QString accessType=cotrollers.at(currentCtrlIndex).toObject()["devices"].
            toArray().at(currentDeviceIndex).toObject()["accessType"].toString();
    qDebug()<<"waterSupplyHandler accessType:"<<accessType;
    if("RW"==accessType||"R"==accessType){
        int ctrlAddress=cotrollers.at(cntrIndex).toObject()["address"].toInt();
        int deviceAddress=cotrollers.at(cntrIndex).
                toObject()["devices"].toArray().
                at(deviceIndex).toObject()["functionAddress"].toInt();
        readTimeout->start();
        QByteArray ctrlAddr;
        ctrlAddr.append(ctrlAddress<<24);
        ctrlAddr.append(ctrlAddress<<16);
        ctrlAddr.append(ctrlAddress<<8);
        ctrlAddr.append(ctrlAddress);
        QByteArray devAddr;
        devAddr.append(deviceAddress<<24);
        devAddr.append(deviceAddress<<16);
        devAddr.append(deviceAddress<<8);
        devAddr.append(deviceAddress);
        return devCtrlCom->read(ctrlAddr,devAddr);
    }else{
        readNextDeviceOrReport();
    }
}
bool WaterSupplyHandler::hasNextDevice()
{
    quint32 nextDeviceIndex = currentDeviceIndex+1;

    if(!deviceExist(currentCtrlIndex,nextDeviceIndex))
    {
        nextDeviceIndex=0;

        quint32 nextCntrlIndex = currentCtrlIndex+1;

        if(!deviceExist(nextCntrlIndex,nextDeviceIndex))
                return false;
    }
    return true;
}

bool WaterSupplyHandler::readNextDevice()
{
    currentDeviceIndex++;
    if(!deviceExist(currentCtrlIndex,currentDeviceIndex))
    {
        currentDeviceIndex=0;
        currentCtrlIndex++;
        if(!deviceExist(currentCtrlIndex,currentDeviceIndex))
                return false;
    }
    return readDevice(currentCtrlIndex,currentDeviceIndex);
}
bool WaterSupplyHandler::readNextDeviceOrReport()
{
    //如果这是该控制器的最后一个功能设备,
    //则将该控制器添加到当前已采集的控制器列表中
    if(!deviceExist(currentCtrlIndex,currentDeviceIndex+1))
    {
       //if(currentCtrlIndex==0||currentCtrlArray.isEmpty())
        int tmp = cotrollers.at(currentCtrlIndex).toObject()["address"].toInt();
        QJsonObject ctrl;

        ctrl["controllerAddress"]=
        cotrollers.at(currentCtrlIndex).toObject()["address"].toInt();;

        ctrl["deviceValues"]=currentDeviceArray;

        currentCtrlArray.append(ctrl);
        //清空currentDeviceArray，为下读取一个控制器中的功能设备做好准备
        while(!currentDeviceArray.isEmpty())
            currentDeviceArray.removeFirst();
    }
    if(hasNextDevice())
    {
        return readNextDevice();

    }
    else    //向服务器报告
    {
        data2Report=currentCtrlArray;
        QJsonDocument doc(data2Report);
        ServerCommHelper * com =Singleton<ServerCommHelper>::instance();
        GatewayReport waterReport;
        waterReport.uuid = UUIDGenerator::getUUID();
        waterReport.buData =doc.toJson(QJsonDocument::Compact);
        waterReport.buType = getBusinessType();
        waterReport.buDataLength = waterReport.buData.size();

        QByteArray sourceDeviceAddress;
        int gatewayAddress=1;   //hard code ~_~
        quint64 controlerAddress=0;
        int deviceAddress=0;

        sourceDeviceAddress.append(gatewayAddress>>24);
        sourceDeviceAddress.append(gatewayAddress>>16);
        sourceDeviceAddress.append(gatewayAddress>>8);
        sourceDeviceAddress.append(gatewayAddress);
        sourceDeviceAddress.append(controlerAddress>>56);
        sourceDeviceAddress.append(controlerAddress>>48);
        sourceDeviceAddress.append(controlerAddress>>40);
        sourceDeviceAddress.append(controlerAddress>>32);
        sourceDeviceAddress.append(controlerAddress>>24);
        sourceDeviceAddress.append(controlerAddress>>16);
        sourceDeviceAddress.append(controlerAddress>>8);
        sourceDeviceAddress.append(controlerAddress);
        sourceDeviceAddress.append(deviceAddress>>24);
        sourceDeviceAddress.append(deviceAddress>>16);
        sourceDeviceAddress.append(deviceAddress>>8);
        sourceDeviceAddress.append(deviceAddress);

        waterReport.deviceAddr = sourceDeviceAddress;

        //返回之前将读取超时计时器停止，打开轮训计时器，断开设备读取槽函数

        startPolling->start();
//        modbus->disconnect(SIGNAL(readFinished(quint64,
//            quint32,const QByteArray &)));
        devCtrlCom->disconnect(SIGNAL(readFinished(QByteArray,QByteArray,QByteArray,bool)));

        return com->report2Server(&waterReport);
    }

}

void WaterSupplyHandler::handleReadFinished(QByteArray controllerAddress,
       QByteArray funcDeviceAddress,const QByteArray &data,bool opResult)
{
    int ctrlAddress=cotrollers.at(currentCtrlIndex).toObject()["address"].toInt();
    int deviceAddress=cotrollers.at(currentCtrlIndex).
             toObject()["devices"].toArray().
             at(currentDeviceIndex).toObject()["functionAddress"].toInt();
    if(controllerAddress.toInt()!=ctrlAddress||
            deviceAddress!=funcDeviceAddress.toInt()){
        return;//不是本模块发起的请求，忽略
    }

    readTimeout->stop();
    QJsonObject device;
    device["functionAddress"]=deviceAddress;
    device["functionValue"]=QString(data);
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss");
    device["timestamp"]=str;
    currentDeviceArray.append(device);

    readNextDeviceOrReport();
}

void WaterSupplyHandler::startPollingDevices()//开始一轮轮训
{
    currentCtrlIndex=0;
    currentDeviceIndex=0;
    if(!deviceExist(currentCtrlIndex,currentDeviceIndex)){
        qDebug()<<"no device exist";
        return;//没有配置任何设备
    }
    while(!data2Report.isEmpty())
        data2Report.removeFirst();
    while(!currentDeviceArray.isEmpty())
        currentDeviceArray.removeFirst();
    while(!currentCtrlArray.isEmpty())
        currentCtrlArray.removeFirst();

    startPolling->stop();
//    connect(modbus,SIGNAL(readFinished(quint64,
//            quint32,const QByteArray&)),
//            this,SLOT(handleReadFinished
//            (quint64, quint32,
//             const QByteArray&)));
    connect(devCtrlCom,SIGNAL(readFinished(QByteArray,QByteArray,QByteArray,bool)),
            this,SLOT(handleReadFinished(QByteArray,QByteArray,QByteArray,bool)));
    readDevice(currentCtrlIndex,currentDeviceIndex);
    ;
//    QJsonObject controller;
//    controller["controllerAddress"]=cotrollers.at(currentCtrlIndex)["address"];
//    controller["deviceValues"]=QJsonArray();
//    data2Report.append(Q);
//    cotrollers[currentCtrlIndex]
}

bool WaterSupplyHandler::init()
{
    //获得当前所有水务设备的地址及访问信息-JSON格式
    if(devManager->getCtrlsAndDevicesInfo(BussinessType::WaterSupply2nd,cotrollers)==false){
       return false;
    }

    startPolling->start();

    //根据水务设备信息，初始化待报告数据-JSON格式

    //连接设备读取成功的信号和处理槽函数

    //设置超时读取超时timer，如果该timer超期，则认为本次读取设备失败，开始下一次读取

    return true;
}
void WaterSupplyHandler::handleDeviceConfigReceived(){
    qDebug()<<"WaterSupplyHandler:handleDeviceConfigReceived()";
    init();
}
