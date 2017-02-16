#include "servercommhelper.h"

quint32 ServerCommHelper::FULL_ADDRESS_LENGTH=16;
quint32 ServerCommHelper::GW_ADDRESS_LENGTH=4;
quint32 ServerCommHelper::CONTROLLER_ADDRESS_LENGTH=8;
quint32 ServerCommHelper::FUNCDEVICE_ADDRESS_LENGTH=4;

ServerCommHelper::ServerCommHelper(QObject *parent) : QObject(parent)
{
    conn=Singleton< ConnectionManager>::instance();
    connected=false;
    connect(conn,SIGNAL(serverMsgReceived(const QByteArray*)),this,SLOT(handleManagerMsgReceived(const QByteArray*)));
    connect(conn,SIGNAL(serverConnected()),this,SLOT(handleServerConnected()));
    connect(conn,SIGNAL(serverDisconnected()),this,SLOT(handleServerDisconnected()));
}
void ServerCommHelper::handleServerConnected(){
    connected=true;
    QString replyDirPath=QDir::currentPath();
    QString replyDirName=replyDirPath.append("/ReplyCache");
    QDir replydir(replyDirName);
    QStringList replyFilter;
    replyFilter<<"*.txt";
    replydir.setNameFilters(replyFilter);
    QStringList replyList=replydir.entryList(replyFilter);
    if(replyList.size()!=0){
        for(int i=0;i<replyList.size();i++){
            QString replyCache="ReplyCache/";
            QFile reply(replyCache.append(replyList.at(i)));
            if(!reply.open(QIODevice::ReadWrite)){
                return;
            }
            QJsonObject jsonObject;
            QJsonParseError jsonError;
            QJsonDocument jsonDocument=QJsonDocument::fromJson(reply.readAll(),&jsonError);
            if(!(jsonDocument.isEmpty()||jsonDocument.isNull())){
                if(jsonError.error!=QJsonParseError::NoError){
                    return;
                }
                jsonObject=jsonDocument.object();
                if(!jsonObject.isEmpty()){
                    quint32 uuid=jsonObject.value("uuid").toInt();
                    quint8 opResult=jsonObject.value("opResult").toInt();
                    quint16 resultLength=jsonObject.value("resultLen").toInt();
                    QByteArray resData=jsonObject.value("resData").toString().toLatin1();
                    qDebug()<<uuid<<" "<<opResult<<" "<<resultLength<<" "<<resData;
                    GatewayResponse *GWResponse=new GatewayResponse();
                    GWResponse->uuid=uuid;
                    GWResponse->opResult=opResult;
                    GWResponse->resultLength=resultLength;
                    GWResponse->resData=resData;
                    reply2Server(GWResponse);
                }
            }
            reply.close();
            reply.remove();
       }
    }

    QString reportDirPath=QDir::currentPath();
    QString reportDirName=reportDirPath.append("/ReportCache");
    QDir reportDir(reportDirName);
    QStringList reportFilter;
    reportFilter<<"*.txt";
    reportDir.setNameFilters(reportFilter);
    QStringList reportList=reportDir.entryList(reportFilter);
    qDebug()<<"reportList:"<<reportList.size();
    if(reportList.size()!=0){
        for(int i=0;i<reportList.size();i++){
            QString replyCache="ReportCache/";
            QFile report(replyCache.append(reportList.at(i)));
            if(!report.open(QIODevice::ReadWrite)){
                return;
            }
            QJsonObject jsonObject;
            QJsonParseError jsonError;
            QJsonDocument jsonDocument=QJsonDocument::fromJson(report.readAll(),&jsonError);
            if(!(jsonDocument.isEmpty()||jsonDocument.isNull())){
                if(jsonError.error!=QJsonParseError::NoError){
                    return;
                }
                jsonObject=jsonDocument.object();
                if(!jsonObject.isEmpty()){
                    quint32 uuid=jsonObject.value("uuid").toInt();
                    QByteArray deviceAddr=jsonObject.value("deviceAddr").toString().toLatin1();
                    BussinessType buType=(BussinessType)jsonObject.value("businessType").toInt();
                    quint16 buDataLen=jsonObject.value("buDataLen").toInt();
                    QByteArray buData=jsonObject.value("buData").toString().toLatin1();
                    qDebug()<<uuid<<" "<<deviceAddr<<" "<<buType<<" "<<buDataLen<<" "<<buData;
                    GatewayReport *rep=new GatewayReport();
                    rep->uuid=uuid;
                    rep->deviceAddr=deviceAddr;
                    rep->buType=buType;
                    rep->buDataLength=buDataLen;
                    rep->buData=buData;
                    report2Server(rep);
                }
            }
            report.close();
            report.remove();
       }
    }
}
void ServerCommHelper::handleServerDisconnected(){
    connected=false;
}

bool ServerCommHelper::reply2Server(GatewayResponse *res){
    GatewayResponse *response=res;
    quint32 uuid=response->uuid;
    quint8 opResult=response->opResult;
    quint16 resultLength=response->resultLength;
    QByteArray resData=response->resData;
    QByteArray UUID;
    UUID.append(uuid>>24);
    UUID.append(uuid>>16);
    UUID.append(uuid>>8);
    UUID.append(uuid);
    QByteArray operatorResult;
    operatorResult.append(opResult);
    QByteArray resultLen;
    resultLen.append(resultLength>>8);
    resultLen.append(resultLength);

    if(connected==false){
        QString dirPath=QDir::currentPath();
        QString dirName=dirPath.append("/ReplyCache");
        QDir dir(dirName);
        if(!dir.exists()){
            bool ok=dir.mkdir(dirName);
            if(ok==false)
                return false;
        }
        QDateTime time=QDateTime::currentDateTime();
        QString fileName=QString("ReplyCache/").append(time.toString("yyyy-MM-dd hh-mm-ss")).append(".txt");
        QFile replyCache(fileName);
        if(!replyCache.open(QIODevice::ReadWrite)){
            return false;
        }
        QJsonObject jsonObject;
        jsonObject.insert("uuid",(int)uuid);
        jsonObject.insert("opResult",opResult);
        jsonObject.insert("resultLen",resultLength);
        jsonObject.insert("resData",QString(resData));
        QJsonDocument jsonDocument;
        jsonDocument.setObject(jsonObject);
        replyCache.write(jsonDocument.toJson());
        replyCache.close();
        return true;
    }

    QByteArray data;
    data.append(UUID);
    data.append(operatorResult);
    data.append(resultLen);
    data.append(resData);

    if(conn->send(data)==true){
        return true;
    }else{
        return false;
    }
}
bool ServerCommHelper::report2Server(GatewayReport *request){
    quint32 uuid=request->uuid;
    QByteArray deviceAddr=request->deviceAddr;
    if(request->buType>2)
        request->buType=BussinessType::UnknownBussinessType;
    BussinessType buType=request->buType;
    quint16 businessDataLength=request->buDataLength;
    QByteArray buData=request->buData;
    QByteArray UUID;
    UUID.append(uuid>>24);
    UUID.append(uuid>>16);
    UUID.append(uuid>>8);
    UUID.append(uuid);
    QByteArray businessType;
    businessType.append(buType>>24);
    businessType.append(buType>>16);
    businessType.append(buType>>8);
    businessType.append(buType);
    QByteArray buDataLen;
    buDataLen.append(businessDataLength>>8);
    buDataLen.append(businessDataLength);

    if(connected==false){
        QString dirPath=QDir::currentPath();
        QString dirName=dirPath.append("/ReportCache");
        QDir dir(dirName);
        if(!dir.exists()){
            bool ok=dir.mkdir(dirName);
            if(ok==false){
                return false;
            }
        }
        QDateTime time=QDateTime::currentDateTime();
        QString fileName=QString("ReportCache/").append(time.toString("yyyy-MM-dd hh-mm-ss")).append(".txt");
        QFile reportCache(fileName);
        if(!reportCache.open(QIODevice::ReadWrite)){
            return false;
        }
        QJsonObject jsonObject;
        jsonObject.insert("uuid",(int)uuid);
        jsonObject.insert("deviceAddr",QString(deviceAddr));
        jsonObject.insert("businessType",buType);
        jsonObject.insert("buDataLen",businessDataLength);
        jsonObject.insert("buData",QString(buData));
        QJsonDocument jsonDocument;
        jsonDocument.setObject(jsonObject);
        reportCache.write(jsonDocument.toJson());
        reportCache.close();
        return true;
    }else{
        QByteArray data;
        data.append(UUID);
        data.append(deviceAddr);
        data.append(businessType);
        data.append(buDataLen);
        data.append(buData);
        if(conn->send(data)==true){
            return true;
        }else{
            return false;
        }
    }
}

bool ServerCommHelper::isValidFullAddress(const QByteArray &deviceAddrs)
{
    if(deviceAddrs.size()!=ServerCommHelper::FULL_ADDRESS_LENGTH){
        return false;
    }
    QByteArray gwAddress;
    QByteArray controllerAddress;
    QByteArray funcDeviceAddress;
    if(getGWAddress(deviceAddrs,gwAddress)&&
            getControllerAddress(deviceAddrs,controllerAddress)&&
            getFuncDeviceAddress(deviceAddrs,funcDeviceAddress))
    {
        if(isValidGWAddress(gwAddress)&&
                isValidControllerAddress(controllerAddress)&&
                isValidFuncDeviceAddress(funcDeviceAddress))
        {
            return true;
        }
    }
    return false;
}

bool ServerCommHelper::isValidGWAddress(const QByteArray &deviceAddrs)
{
    bool ok;
    if(deviceAddrs.length()!=GW_ADDRESS_LENGTH)
        return false;
    quint32 gwAddress=deviceAddrs.toHex().toUInt(&ok);
    if(ok==false){
        return false;
    }
    if(gwAddress==0)
    {
        return false;
    }
    return true;
}

bool ServerCommHelper::isValidControllerAddress(const QByteArray &deviceAddrs)
{
    bool ok;
    if(deviceAddrs.length()!=CONTROLLER_ADDRESS_LENGTH)
        return false;
    quint64 controlAddress=deviceAddrs.toHex().toULong(&ok,16);
    if(ok==false){
        return false;
    }
    if(controlAddress==0)
    {
        return false;
    }
    return true;
}

bool ServerCommHelper::isValidFuncDeviceAddress(const QByteArray &deviceAddrs)
{
    bool ok;
    quint32 funcDeviceAddress=deviceAddrs.toHex().toUInt(&ok);
    if(ok==false){
        return false;
    }
    if(funcDeviceAddress==0)
    {
        return false;
    }
    return true;

}

bool ServerCommHelper::getGWAddress(const QByteArray &deviceAddrs, QByteArray &gwAddrs)
{
    bool ok;
    quint32 gwAddress=deviceAddrs.left(GW_ADDRESS_LENGTH).toHex().toInt(&ok);
    if(ok==false)
        return false;
    if(gwAddress==0)
    {
        return false;
    }
    gwAddrs.append(gwAddress>>24);
    gwAddrs.append(gwAddress>>16);
    gwAddrs.append(gwAddress>>8);
    gwAddrs.append(gwAddress);
    return true;
}

bool ServerCommHelper::getControllerAddress(const QByteArray &deviceAddrs, QByteArray &controllerAddrs)
{
    bool ok;
    quint64 controlAddress=deviceAddrs.mid(GW_ADDRESS_LENGTH,CONTROLLER_ADDRESS_LENGTH).toHex().toULong(&ok,16);
    if(ok==false)
        return false;
    if(controlAddress==0)
    {
        return false;
    }
    controllerAddrs.append(controlAddress>>56);
    controllerAddrs.append(controlAddress>>48);
    controllerAddrs.append(controlAddress>>40);
    controllerAddrs.append(controlAddress>>32);
    controllerAddrs.append(controlAddress>>24);
    controllerAddrs.append(controlAddress>>16);
    controllerAddrs.append(controlAddress>>8);
    controllerAddrs.append(controlAddress);
    return true;
}

bool ServerCommHelper::getFuncDeviceAddress(const QByteArray &deviceAddrs, QByteArray &funcAddrs)
{
    bool ok;
    quint32 funcDeviceAddress=deviceAddrs.right(FUNCDEVICE_ADDRESS_LENGTH).toHex().toInt(&ok);
    if(ok==false)
        return false;
    if(funcDeviceAddress==0)
    {
        return false;
    }
    funcAddrs.append(funcDeviceAddress>>24);
    funcAddrs.append(funcDeviceAddress>>16);
    funcAddrs.append(funcDeviceAddress>>8);
    funcAddrs.append(funcDeviceAddress);
    return true;
}

void ServerCommHelper::handleManagerMsgReceived(const QByteArray *sendData){
    if(connected==false)
        return;
    QByteArray data=*sendData;
    if(data.size()<7){
        return;
    }
    QByteArray id=data.left(4);
    quint8 first=id[0];
    if(((first>>7)&1)!=0){
        //服务器响应
        ServerResponse* response=new ServerResponse();
        bool ok;
        data.remove(0,4);
        QByteArray operatorResult=data.left(1);
        data.remove(0,1);
        QByteArray resultLen=data.left(2);
        data.remove(0,2);
        QByteArray resData=data;
        quint32 uuid=id.toHex().toUInt(&ok,16);
        //不符合要求的直接丢掉
        if(ok==false){
            return;
        }
        quint8 opResult=(quint8)operatorResult[0];
        quint16 resultLength=resultLen.toHex().toUShort(&ok,16);
        if(ok==false){
            return;
        }
        if(resData.size()!=resultLength){
            return;
        }
        qDebug()<<"server response";
        response->uuid=uuid;
        response->opResult=opResult;
        response->resultLength=resultLength;
        response->resData=resData;
        qDebug()<<"serverCommHelper:"<<response->uuid<<" "<<response->opResult<<" "<<response->resultLength<<" "<<response->resData;

        emit serverResponseReceived(response);
    }else{
        if(data.size()<27){
            return;
        }
        //服务器请求
        ServerRequest* request=new ServerRequest();
        bool ok;

        data.remove(0,4);
        QByteArray deviceAddr=data.left(16);
        data.remove(0,16);
        QByteArray bussinessType=data.left(4);
        data.remove(0,4);
        QByteArray operatorType=data.left(1);
        data.remove(0,1);
        QByteArray bussinessDataLength=data.left(2);
        data.remove(0,2);
        QByteArray buData=data.left(data.length());
        quint32 uuid=id.toHex().toUInt(&ok,16);
        //不符合要求直接丢掉
        if(ok==false){
            return;
        }
        quint32 buType=bussinessType.toHex().toUInt(&ok,16);
        if(ok==false){
            return;
        }
        if(buType>2)
            buType=100;

        quint8 opType=operatorType.toHex().toUInt(&ok,16);
        if(ok==false){
            return;
        }
        quint16 buDataLength=bussinessDataLength.toHex().toUShort(&ok,16);
        if(ok==false){
            return;
        }
        if(buDataLength!=buData.size()){
            return;
        }
        qDebug()<<"server request";
        request->uuid=uuid;
        request->deviceAddr=deviceAddr;
        request->buType=(BussinessType)buType;
        request->opType=(OperationType)opType;
        request->buDataLength=buDataLength;
        request->buData=buData;
        qDebug()<<"serverCommHelper:"<<request->uuid<<" "<<request->deviceAddr.toHex()<<" "<<request->buType<<" "<<request->opType<<" "<<request->buDataLength<<" "<<request->buData;
        emit serverRequestReceived(request);
        delete request;
    }
}
