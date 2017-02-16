#ifndef IBUSINESSHANDLER_H
#define IBUSINESSHANDLER_H
#include "qbytearray.h"
#include "commontypesdef.h"
class IBusinessHandler
{

public:

virtual bool init()=0;
virtual bool handleWriteMsg(const  QByteArray &targetAddress,const  QByteArray &data)=0;
virtual bool handleReadMsg(const  QByteArray  &targetAddress, QByteArray & replyData)=0;

/*返回为true表示通过筛选， GWEngine将继续处理该写消息；否则终止处理，使用returnValue的值作为返回给服务器端的值*/
virtual bool filterWriteMsg(const  QByteArray &source, bool &returnValue)=0;

/*返回为true表示通过筛选， GWEngine将继续处理该写消息；否则终止处理，使用returnValue的值作为返回给服务器端的值*/
virtual bool filterReadMsg(const  QByteArray &source, bool &returnValue,QByteArray & replyData)=0;
virtual BussinessType getBusinessType()=0;
    virtual ~IBusinessHandler(){}
};

#endif
