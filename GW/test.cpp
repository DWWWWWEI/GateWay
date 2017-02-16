#include "test.h"
#include "connectionmanager.h"
#include "uuidgenerator.h"
#include "businesshandlerfactory.h"
Test::Test(){
    startTimer(25000);
    helper=new ServerCommHelper();
}
void Test::startWaterSuply()
{
    static bool first=true;
    if(first){
    IBusinessHandler *iBuss =
            BusinessHandlerFactory::getBusinessHandler(BussinessType::WaterSupply2nd);
    iBuss->init();
    first = false;
    }
    return;
}

void Test::timerEvent(QTimerEvent *){
    //网关请求
    qDebug()<<"gateway request";
    GatewayReport* report=new GatewayReport;
    quint32 uuid=UUIDGenerator::getUUID();
    QByteArray deviceAddr;
    int gatewayAddress=1;
    quint64 controlerAddress=4;
    int deviceAddress=2;
    deviceAddr.append(gatewayAddress>>24);
    deviceAddr.append(gatewayAddress>>16);
    deviceAddr.append(gatewayAddress>>8);
    deviceAddr.append(gatewayAddress);
    deviceAddr.append(controlerAddress>>56);
    deviceAddr.append(controlerAddress>>48);
    deviceAddr.append(controlerAddress>>40);
    deviceAddr.append(controlerAddress>>32);
    deviceAddr.append(controlerAddress>>24);
    deviceAddr.append(controlerAddress>>16);
    deviceAddr.append(controlerAddress>>8);
    deviceAddr.append(controlerAddress);
    deviceAddr.append(deviceAddress>>24);
    deviceAddr.append(deviceAddress>>16);
    deviceAddr.append(deviceAddress>>8);
    deviceAddr.append(deviceAddress);
    BussinessType buType=BussinessType::ServerConnection;
    QByteArray buData;
    buData.append(63);
    buData.append(64);
    quint16 buDataLength=buData.length();
    report->uuid=uuid;
    report->deviceAddr=deviceAddr;
    report->buType=buType;
    report->buDataLength=buDataLength;
    report->buData=buData;
    helper->report2Server(report);

    //网关响应
//    qDebug()<<"gateway response";
//    GatewayResponse* response=new GatewayResponse;
//    quint32 uuid=UUIDGenerator::getUUID();
//    quint8 opResult=1;
//    QByteArray resData;
//    resData.append(53);
//    resData.append(42);
//    quint16 resultlength=resData.length();
//    response->uuid=uuid;
//    response->opResult=opResult;
//    response->resultLength=resultlength;
//    response->resData=resData;
//    helper->reply2Server(response);
}
