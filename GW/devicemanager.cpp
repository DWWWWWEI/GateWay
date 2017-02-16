#include "devicemanager.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
QString DeviceManager::DEVICE_IS_HEALTH = "OK";
QString DeviceManager::DEVICE_IS_NON_HEALTH= "NOK";
bool DeviceManager::isInitialized=false;

DeviceManager::DeviceManager(QObject *parent) : QObject(parent)
{
    if(isInitialized==false){
        init();
    }
}
bool DeviceManager::init(){
    if(isInitialized==false){
        if(loadDeviceConfig()){
            return true;
            isInitialized=true;
        }else{
            return false;
        }
    }
    return false;
}

bool DeviceManager::initDeviceConfig()
{
    if(loadDeviceConfig()==false){
        create1stConfigFile();
        if(loadDeviceConfig()==false)//说明创建失败了
        {
            qCritical()<<"初始化设备配置文件失败";
            return false;
        }
    }
    return true;
}

bool DeviceManager::loadDeviceConfig()
{
    QFile loadFile(QStringLiteral("DeviceConfig.json"));
          if (!loadFile.open(QIODevice::ReadOnly)) {
              qWarning("Couldn't open save file.");
              return false;
          }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    cotrollers =loadDoc.array();
    return true;
}

bool DeviceManager::create1stConfigFile()
{
    QJsonObject sensor1{
        {"functionAddress", 1},
        {"functionname", "temprature"}
    };
    QJsonObject sensor2{
        {"functionAddress", 2},
        {"functionname", "pressure"}
    };
    QJsonObject sensor3;
    sensor3["functionAddress"]=3;
    sensor3["functionname"]="light";
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

bool DeviceManager::createConfigFile(QJsonDocument &doc)
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
bool DeviceManager::getCtrlsAndDevicesInfo(BussinessType buType, QJsonArray &controllerList){
    QFile loadFile(QStringLiteral("DeviceConfig.json"));
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonArray localArray =loadDoc.array();
    controllerList = localArray;
    int deviceNum =controllerList.at(0).toObject()["devices"].toArray().size();
    return true;
}

bool DeviceManager::handleWriteMsg(const QByteArray &targetAddress, const QByteArray &data){
    QJsonObject jsonObject;
    QJsonParseError jsonError;
    QJsonDocument jsonDocument=QJsonDocument::fromJson(data,&jsonError);
    if(!(jsonDocument.isEmpty()||jsonDocument.isNull())){
        if(jsonError.error!=QJsonParseError::NoError){
            return false;
        }
        jsonObject=jsonDocument.object();
    }
    qDebug()<<jsonObject.value("dataType").toString();
    if("Object"==jsonObject.value("dataType").toString()){
        jsonObject=jsonObject.value("dataContent").toObject();
        if("set"==jsonObject.value("operation").toString()){
            QJsonArray controllers=jsonObject.value("controllers").toArray();
            QFile loadFile(QStringLiteral("DeviceConfig.json"));
            if(!loadFile.open(QIODevice::WriteOnly)){
                qWarning("Couldn't open save file.");
                return false;
            }
            QJsonDocument doc;
            doc.setArray(controllers);
            loadFile.write(doc.toJson());
            loadFile.close();
            emit receiveDeviceConfig();
            return true;
        }
    }
}
