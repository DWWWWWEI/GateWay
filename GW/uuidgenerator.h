#ifndef UUIDGENERATOR_H
#define UUIDGENERATOR_H
#include <QObject>
#include <QTime>
class UUIDGenerator
{
public:
    static quint32 getUUID();
    static const QByteArray& transFormToByteArray(quint32 uuid);
private:
    static QByteArray UUID;
    UUIDGenerator();
};

#endif // UUIDGENERATOR_H
