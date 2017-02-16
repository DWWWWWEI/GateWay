#ifndef SERVERCOMMHELPER_H
#define SERVERCOMMHELPER_H

#include <QObject>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDir>
#include <QCoreApplication>
#include <QDateTime>
#include "commontypesdef.h"
#include "connectionmanager.h"
class ServerRequest{
public:
    quint32 uuid;
    QByteArray deviceAddr;
    BussinessType buType;
    OperationType opType;
    quint16 buDataLength;
    QByteArray buData;
};
class ServerResponse{
public:
    quint32 uuid;
    quint8 opResult;
    quint16 resultLength;
    QByteArray resData;
};
class GatewayReport{
public:
    quint32 uuid;
    QByteArray deviceAddr;
    BussinessType buType;
    quint16 buDataLength;
    QByteArray buData;
};
class GatewayResponse{
public:
    quint32 uuid;
    quint8 opResult;
    quint16 resultLength;
    QByteArray resData;
};
class ServerCommHelper : public QObject
{
    Q_OBJECT
public:
    explicit ServerCommHelper(QObject *parent = 0);
    bool reply2Server(GatewayResponse *);
    bool report2Server(GatewayReport *);

    static bool isValidFullAddress(const QByteArray &deviceAddrs);
    static bool isValidGWAddress(const QByteArray &deviceAddrs);
    static bool isValidControllerAddress(const QByteArray &deviceAddrs);
    static bool isValidFuncDeviceAddress(const QByteArray &deviceAddrs);

    static bool getGWAddress(const QByteArray &deviceAddrs, QByteArray &gwAddrs);
    static bool getControllerAddress(const QByteArray &deviceAddrs, QByteArray &controllerAddrs);
    static bool getFuncDeviceAddress(const QByteArray &deviceAddrs, QByteArray &funcAddrs);
    static quint32 FULL_ADDRESS_LENGTH;
    static quint32 GW_ADDRESS_LENGTH;
    static quint32 CONTROLLER_ADDRESS_LENGTH;
    static quint32 FUNCDEVICE_ADDRESS_LENGTH;
signals:
    void serverRequestReceived(const ServerRequest *);
    void serverResponseReceived(const ServerResponse *);
private:
    ConnectionManager* conn;
    bool connected;
public slots:
    void handleServerConnected();
    void handleServerDisconnected();
    void handleManagerMsgReceived(const QByteArray* data);
    friend class Singleton<ServerCommHelper>;
};

#endif // SERVERCOMMHELPER_H
