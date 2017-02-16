#ifndef TEST_H
#define TEST_H
#include <QObject>
#include <QTimer>
#include "servercommhelper.h"
class Test:public QObject{
    Q_OBJECT
public:
    Test();
    void timerEvent(QTimerEvent *);
    ServerCommHelper *helper;
public slots:
    void startWaterSuply();
};

#endif // TEST_H
