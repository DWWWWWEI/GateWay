#include <connectionmanager.h>
ConnectionManager* ConnectionManager::connectionManager=NULL;
//初始化Socket
ConnectionManager::ConnectionManager(QObject *parent):QObject(parent){
    connectionToServerSocket=new QTcpSocket;
}
ConnectionManager::~connectionManager(){
}

//连接通信服务器，并打开心跳线程
bool ConnectionManager::start(const QString &ip, quint16 port){
    connectionToServerSocket->connectToHost(ip,port);
    connect(connectionToServerSocket,SIGNAL(readyRead()),this,SLOT(serverMsgHandle()));
    connect(connectionToServerSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(isSendBytesWritten(qint64)));
    //ConnectionManager打开后60秒发送一个注册包
    isRegister=true;
    heartBeatTimer=startTimer(10000);
    if(connectionToServerSocket->isOpen()==true){
        return true;
    }else{
        return false;
    }
}
//超时处理
void ConnectionManager::timerEvent(QTimerEvent *event){
    if(event->timerId()==heartBeatTimer){
       UUID=UUIDGenerator::transFormToByteArray(UUIDGenerator::getUUID());

       QByteArray sourceDeviceAddress;
       int gatewayAddress=1;
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

       QByteArray businessType;
       int intBusinssType=0;
       businessType.append(intBusinssType>>24);
       businessType.append(intBusinssType>>16);
       businessType.append(intBusinssType>>8);
       businessType.append(intBusinssType);

       QByteArray businessDataLength;
       quint16 intBusinessDataLength=0;
       businessDataLength.append(intBusinessDataLength>>8);
       businessDataLength.append(intBusinessDataLength);

       QByteArray PDU;
       PDU.append(UUID);
       PDU.append(sourceDeviceAddress);
       PDU.append(businessType);
       PDU.append(businessDataLength);

       send(PDU);
       replyTimer=startTimer(30000);
    }else if(event->timerId()==replyTimer){
        //如果隔30秒还未收到服务器的响应，就认为断线了
        qDebug()<<"server is die";
        connectionToServerSocket->close();
        isRegister=false;
        killTimer(replyTimer);
        killTimer(heartBeatTimer);
        emit serverDisconnected();
    }
}

//断开与服务器连接
bool ConnectionManager::stop(){
    connectionToServerSocket->close();
    if(connectionToServerSocket->isOpen()==false){
        return false;
    }else{
        return false;
    }
}
bool ConnectionManager::setServerAddrs(const QString &ip, quint16 port){
    return false;
}
//封装PDU,发送给服务器
bool ConnectionManager::send(const QByteArray &source){
    QByteArray frameHeader="HKQY";

    QByteArray frameData=source;
    quint16 length=frameData.length();
    QByteArray frameDataLength;
    frameDataLength.append(length>>8);
    frameDataLength.append(length);

    quint8 sum=0;
    for(int i=0;i<frameData.length();i++){
        sum=(quint8)(sum+frameData[i]);
    }
    QByteArray frameSumCheck;
    frameSumCheck.append(sum);

    QByteArray frameTailer="BZKJ";

    QByteArray frame;
    frame.append(frameHeader);
    frame.append(frameDataLength);
    frame.append(frameData);
    frame.append(frameSumCheck);
    frame.append(frameTailer);
    qDebug()<<frame.toHex();
    connectionToServerSocket->write(frame);
    connectionToServerSocket->flush();
    if(bytesWritten!=true){
        qDebug()<<"send failed";
        return false;
    }else{
        qDebug()<<"send success";
        bytesWritten=false;
        return true;
    }
}
//判断消息是否写入
void ConnectionManager::isSendBytesWritten(qint64 bytesLength){
    if(bytesLength>0){
       bytesWritten=true;
    }else{
       bytesWritten=false;
    }
}
//接收通信服务器发送的消息
void ConnectionManager::serverMsgHandle(){
    QByteArray data=connectionToServerSocket->readAll();
    qDebug()<<data.toHex();
    //判断帧头是否为HKQY
    QByteArray frameHeader=data.left(4);
    if(frameHeader!="HKQY"){
        qDebug()<<"it is`t start with HKQY";
        return;
    }
    //判断长度是否为数字
    data.remove(0,4);
    bool ok;
    QByteArray dataLength=data.left(2).toHex();\
    short frameDataLength=dataLength.toShort(&ok,16);
    if(ok!=true){
        qDebug()<<"it is`t a number";
        return;
    }
    //获得数据
    data.remove(0,2);
    QByteArray frameData=data.left(frameDataLength);

    //获得校验和
    data.remove(0,frameDataLength);
    QByteArray frameSumCheck=data.left(1);
    quint8 sum=0;
    for(int i=0;i<frameData.length();i++){
        sum=(quint8)(sum+frameData[i]);
    }
    QByteArray sumArr;
    sumArr.append(sum);
    if(frameSumCheck!=sumArr){
        qDebug()<<"the sum is wrong";
        return;
    }
    data.remove(0,1);

    //检查帧尾
    QByteArray frameTailer=data.left(4);
    if(frameTailer!="BZKJ"){
        qDebug()<<"it is`t end with BZKJ";
        return;
    }

    QByteArray frameUUID=frameData.left(4);
    quint8 first=frameUUID[0];
    //如果服务器发送数据，就删除定时器
    if(replyTimer){
        killTimer(replyTimer);
        replyTimer=0;
    }
    if(heartBeatTimer){
        killTimer(heartBeatTimer);
        heartBeatTimer=startTimer(40000);
    }
    if(((first>>7)&1)!=0){
        if(UUID==frameUUID){
            UUID=Q_NULLPTR;
            if(isRegister==true){
//                frameData.remove(0,4);
//                int opResult=frameData.left(1).toHex().toInt(nullptr,16);
//                if(opResult!=0){
//                    isRegister=false;
//                    qDebug()<<"-------- server say success";
                    emit serverConnected();
//                }
//                qDebug()<<"-------- server say false";
            }
            //服务器回复心跳包
            qDebug()<<"it`s reply of heartbeat";
        }else{
            emit serverMsgReceived(&frameData);
        }
    }else{
        emit serverMsgReceived(&frameData);
    }
}
