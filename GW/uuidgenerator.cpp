#include "uuidgenerator.h"
QByteArray UUIDGenerator::UUID;
UUIDGenerator::UUIDGenerator()
{
}
quint32 UUIDGenerator::getUUID(){
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    quint32 uuid=qrand()%RAND_MAX;
    uuid|=1<<31;
    return uuid;
}
const QByteArray& UUIDGenerator::transFormToByteArray(quint32 uuid){\
    UUID.clear();
    UUID.append(uuid>>24);
    UUID.append(uuid>>16);
    UUID.append(uuid>>8);
    UUID.append(uuid);
    return UUID;
}
