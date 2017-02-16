#include "gwengine.h"

#include "connectionmanager.h"
#include "modbushandler.h"
#include "protocolhandlerfactory.h"
#include "servercommhelper.h"
#include "uuidgenerator.h"
#include "test.h"
#include "ibusinesshandler.h"
#include "businesshandlerfactory.h"
#include <QSettings>
#include <QDebug>
#include "devicecontrollercom.h"
GWEngine::GWEngine(QObject *parent) : QObject(parent)
{

}
bool GWEngine::start()
{
    QSettings *configIniRead = new QSettings("Test.ini", QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    QString ip = configIniRead->value("/server/ip").toString();
    int port = configIniRead->value("/server/port").toInt();
    delete configIniRead;
    ConnectionManager *conn=Singleton< ConnectionManager>::instance();
    //QString ip="192.168.0.159";
    qDebug()<<"Server ip is "<<ip;
    qDebug()<<"Server port is "<<port;

    ServerCommHelper * helper = Singleton<ServerCommHelper>::instance();

    QObject::connect(conn, SIGNAL(serverConnected()),this, SLOT(startBusinessHandlers()));

    QObject::connect(helper, SIGNAL(serverRequestReceived(const ServerRequest *)),this, SLOT(handleServerRequest(const ServerRequest *)));

    QObject::connect(helper, SIGNAL(serverResponseReceived(const ServerResponse *)),this, SLOT(handleServerResponse(const ServerResponse *)));

    conn->start(ip,port);

}
void GWEngine::startBusinessHandlers()
{
    static bool first=true;
    if(first){
        QList<IBusinessHandler *> iBizList =
            BusinessHandlerFactory::getAllBizHandler();
        while(!iBizList.isEmpty())
            iBizList.takeFirst()->init();
        first = false;
    }
    return;
}
void GWEngine::handleDeviceReadFinished(const QByteArray&controllerAddress,
                                        const QByteArray&funcDeviceAddress,
                                        const QByteArray&data,bool operationResult)
{
    DeviceControllerCom *deviceCom = Singleton<DeviceControllerCom>::instance();
    if(controllerAddress==operatingControllerAddrs&&
            funcDeviceAddress==operatingFuncDeviceAddrs)
    {
        disconnect(deviceCom, SIGNAL(readFinished(const QByteArray&,const QByteArray&,
                                                  const QByteArray&,bool)),
                   this, SIGNAL(handleDeviceReadFinished(const QByteArray&,const QByteArray&,
                                                          const QByteArray&,bool)));
        handleDeviceOperationFinished(controllerAddress,funcDeviceAddress,
                                     data,operationResult);
    }
    else
        return;
}

void GWEngine::handleDeviceWriteFinished(const QByteArray &controllerAddress,const QByteArray &funcDeviceAddress,
                                         bool operationResult)
{
    DeviceControllerCom *deviceCom = Singleton<DeviceControllerCom>::instance();
    if(controllerAddress.toULong()==operatingControllerAddrs.toHex().toULong(nullptr,16)&&
            funcDeviceAddress.toUInt()==operatingFuncDeviceAddrs.toHex().toUInt(nullptr,16))
    {
        disconnect(deviceCom, SIGNAL(writeFinished(const QByteArray&,const QByteArray&,
                                                  bool)),
                   this, SIGNAL(handleDeviceWriteFinished(const QByteArray&,const QByteArray&,
                                                          bool)));
        QByteArray fakeData;
        handleDeviceOperationFinished(controllerAddress,funcDeviceAddress,
                                     fakeData,operationResult);
    }
    else{
        return;
    }
}

void GWEngine::handleDeviceOperationFinished(const QByteArray& controllerAddress,
                                             const QByteArray& funcDeviceAddress,
                                             const QByteArray& data,bool operationResult)
{
    GatewayResponse servResponse;
    operatingControllerAddrs.clear();
    operatingFuncDeviceAddrs.clear();

    servResponse.opResult = operationResult;
    servResponse.resultLength=data.size();
    servResponse.resData=data;
    servResponse.uuid = handlingServMsgUUID;

    handlingServMsgUUID = 0;
    ServerCommHelper * helper = Singleton<ServerCommHelper>::instance();
    helper->reply2Server(&servResponse);
}

void GWEngine::handleServerRequest(const ServerRequest * serverReq)
{
    IBusinessHandler * handler =
            BusinessHandlerFactory::getBusinessHandler(serverReq->buType);
    qDebug()<<"buType:"<<serverReq->buType;

    GatewayResponse response;
    bool ret=false;
    if(ServerCommHelper::isValidFullAddress(serverReq->deviceAddr))
    {
        //服务器发起的读写请求中包含一个有效的设备全地址，说明是针对某一个具体功能设备的请求
        //这里将通过业务组件的过滤后，由Engine发起对设备的读写操作，并根据异步的操作结果
        //返回对服务器的响应
        qDebug()<<"valid fulladdress";
        DeviceControllerCom *deviceCom = Singleton<DeviceControllerCom>::instance();
        QByteArray controllerAddress;
        QByteArray funcDeviceAddress;
        if(ServerCommHelper::getControllerAddress(serverReq->deviceAddr,controllerAddress)==false)
        {
                qWarning()<<"getControllerAddress error from address "<<serverReq->deviceAddr;
        }
        else if(ServerCommHelper::getFuncDeviceAddress(serverReq->deviceAddr,funcDeviceAddress)==false)
        {
                qWarning()<<"getFuncDeviceAddress error from address "<<serverReq->deviceAddr;
        }
        else
        {
            if(serverReq->opType==Read){
                ret = deviceCom->read(controllerAddress,funcDeviceAddress);
            }
            else if(serverReq->opType==Write){
                QJsonParseError jsonError;
                QJsonDocument doc=QJsonDocument::fromJson(serverReq->buData,&jsonError);
                QJsonObject obj=doc.object();
                if(jsonError.error!=QJsonParseError::NoError){
                    return;
                }
                QByteArray data;
                if("Number"==obj.value("dataType").toString()){
                    short num=obj.value("dataContent").toInt();
                    data.append(num>>8);
                    data.append(num);
                    qDebug()<<"------  GWEngine dataContent "<<data;
                }
                ret = deviceCom->write(controllerAddress,funcDeviceAddress,data);
            }
            else    //unknown operation
                ;
            if(ret){
                //异步读写操作成功，暂时不回复server， 待设备读写完成时再回复
                operatingControllerAddrs = controllerAddress;
                operatingFuncDeviceAddrs = funcDeviceAddress;
                handlingServMsgUUID = serverReq->uuid;
                if(serverReq->opType==Read)
                    connect(deviceCom, SIGNAL(readFinished(const QByteArray&,const QByteArray&,
                                                       const QByteArray&,bool)),
                        this, SLOT(handleDeviceReadFinished(const QByteArray&,const QByteArray&,
                                                               const QByteArray&,bool)));
                else
                    connect(deviceCom, SIGNAL(writeFinished(const QByteArray&,const QByteArray&,
                                                            bool)),
                            this, SLOT(handleDeviceWriteFinished(const QByteArray&,const QByteArray&,
                                                                   bool)));
                return;
            }

        }
    }
    else
    {

        //服务器发起的读写请求中没有包含一个有效的设备全地址，说明是针对业务组件的请求
        //因此将数据转发给业务组件处理，同时根据返回值对服务器进行响应；
        //目前，在该处理分支下只支持业务组件以同步非阻塞的方式返回处理结果
        qDebug()<<"invalid fulladdress";
        if(serverReq->opType==Read){
            ret = handler->handleReadMsg(serverReq->deviceAddr, response.resData);

        }
        else if(serverReq->opType==Write){
            ret = handler->handleWriteMsg(serverReq->deviceAddr,serverReq->buData);
        }
        else    //unknown operation
            ;
    }

    response.opResult=ret;
    response.uuid = serverReq->uuid;
    response.resultLength = response.resData.size();

    ServerCommHelper * helper = Singleton<ServerCommHelper>::instance();
    helper->reply2Server(&response);
}
void GWEngine::handleServerResponse(const ServerResponse * response){

}
