#ifndef GWENGINE_H
#define GWENGINE_H

#include <QObject>
#include "singleton.h"
#include "servercommhelper.h"

class GWEngine : public QObject
{
    Q_OBJECT
public:
    explicit GWEngine(QObject *parent = 0);
    //启动引擎，该函数内部完成网关软件其他对象的启动和初始化
    bool start();
    friend class Singleton<GWEngine>;

signals:

public slots:
    void handleServerRequest(const ServerRequest *);
    void handleServerResponse(const ServerResponse *);
    void startBusinessHandlers();

private:
    QByteArray operatingControllerAddrs;
    QByteArray operatingFuncDeviceAddrs;
    quint32 handlingServMsgUUID;
private slots:

    void handleDeviceReadFinished(const QByteArray&controllerAddress,
                                            const QByteArray&funcDeviceAddress,
                                            const QByteArray&data,bool operationResult);

    void handleDeviceWriteFinished(const QByteArray&controllerAddress,
                                            const QByteArray&funcDeviceAddress,
                                            bool operationResult);


    void handleDeviceOperationFinished(const QByteArray& controllerAddress,
                                                 const QByteArray& funcDeviceAddress,
                                                 const QByteArray& data,bool operationResult);
};

#endif // GWENGINE_H
