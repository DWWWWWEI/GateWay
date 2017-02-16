#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H
#include <QObject>
#include <QDebug>
#include <QtNetwork/QTcpSocket>
#include <QTimerEvent>
#include "singleton.h"
#include "uuidgenerator.h"
class ConnectionManager:public QObject{
    Q_OBJECT
private:
    explicit ConnectionManager(QObject *parent = 0);
    ~connectionManager();
public:
    //static ConnectionManager * getInstance();
    //ConnectionManger被构造完成后，调用该方法开始进行连接
    bool start(const QString &ip, quint16 port);
    bool stop(); //断开与服务器连接
    bool setServerAddrs(const QString &ip, quint16 port);
    bool send(const  QByteArray &source);	//异步
signals:
    void serverDisconnected();
    void serverConnected();
    void serverMsgReceived(const QByteArray* data);
    friend class Singleton<ConnectionManager>;
private slots:
    //处理服务器发送过来的消息，并解帧
    void serverMsgHandle();
    //判断发送是否
    void isSendBytesWritten(qint64 bytesLength);
    void timerEvent(QTimerEvent *event);
private:
    QByteArray UUID;
    int heartBeatTimer;
    int replyTimer;
    bool isRegister;
    bool bytesWritten;
    static ConnectionManager* connectionManager;
    QTcpSocket *connectionToServerSocket;
};
#endif // CONNECTIONMANAGER_H
