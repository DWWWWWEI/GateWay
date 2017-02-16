#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include "ibusinesshandler.h"
#include "singleton.h"
#include <QJsonArray>
/************************************************************************
***在未来的实现中，设备管理信息中需要包含对网关的硬件信息做出的完整的说明********************
***譬如，网关有几个串口，每个串口分别挂载的有哪些modbus或者其他类型的设备.************
*************************************************************************/

class DeviceManager : public QObject,public IBusinessHandler
{
    Q_OBJECT
private:
    explicit DeviceManager(QObject *parent = 0);
public:
    virtual bool init();
    virtual bool handleWriteMsg(const  QByteArray &targetAddress,const  QByteArray &data);
    virtual bool handleReadMsg(const  QByteArray  &targetAddress, QByteArray & replyData){return true;}

    //返回为true表示通过筛选， GWEngine将继续处理该写消息；否则终止处理，使用returnValue的值作为返回给服务器端的值
    virtual bool filterWriteMsg(const  QByteArray &source, bool &returnValue){return true;}

    //返回为true表示通过筛选， GWEngine将继续处理该写消息；否则终止处理，使用returnValue的值作为返回给服务器端的值
    virtual bool filterReadMsg(const  QByteArray &source, bool &returnValue,QByteArray & replyData){return true;}
    virtual BussinessType getBusinessType(){return DeviceManagement;}
    virtual ~DeviceManager(){}
    friend class Singleton<DeviceManager>;

    quint32  getControllerBusinessType(const QString & controller){;}//通过控制器地址查询所属的业务ID
    QString getControllerCommunicationType(const QString &controllerAddress){;}//通过控制器地址查询承载方式，返回的字符串的含义请参考设备管理系统，配置下发消息的JSON定义
    QString getControllerProtocolType(const QString &controllerAddress){;}//通过控制器地址查询协议类型，返回的字符串的含义请参考设备管理系统，配置下发消息的JSON定义
    QString getDeviceHealthStatus(const QString &controllerAddress,const QString &funcDeviceAddress){;}//通过设备地址查询设备健康状态
    static QString DEVICE_IS_HEALTH;
    static QString DEVICE_IS_NON_HEALTH;
    bool getAllCtrlsAndDevicesInfo(QJsonArray&);
    bool getCtrlsAndDevicesInfo(BussinessType,QJsonArray&);

    bool getChildrenInfo(const QString &controllerAddress, QJsonArray&);
    bool getParentInfo(const QString &funcDeviceAddress,QJsonObject );
    signals:
    void deviceHealthReport(const QJsonArray & statusArray);

private:
    bool initDeviceConfig();
    bool loadDeviceConfig();
    bool create1stConfigFile();
    bool createConfigFile(QJsonDocument &doc);

    static bool isInitialized;
    QJsonArray cotrollers;
    QJsonArray ctrlsStatus;
signals:
    void receiveDeviceConfig();
public slots:
};
#endif
