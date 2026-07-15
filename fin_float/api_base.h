#ifndef APIBASE_H
#define APIBASE_H

#include "share_def.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

//通过api获取信息
typedef struct
{
    ShowTargetInfo info;    //获取到的信息
    int32_t step_flag;      //步骤记录,若api需要分多部获取所有数据可使用,为0表示获取完成
}APIGetInfo;

//API基类
class ApiBase : public QObject
{
    Q_OBJECT
public:
    explicit ApiBase(QObject *parent = nullptr)
    {
        Q_UNUSED(parent)
    };
    virtual ~ApiBase(){};

    //虚函数默认返回-1,未实现的函数通过返回值进行判断
    
    //获取当前信息
    virtual int get_current_info(APIGetInfo &get_info, QNetworkAccessManager *manager)
    {
        Q_UNUSED(get_info)
        Q_UNUSED(manager)
        return -1;
    };
    //接收数据处理
    virtual int handle_reply(APIGetInfo &get_info, QNetworkReply *reply)
    {
        Q_UNUSED(get_info)
        Q_UNUSED(reply)
        return -1;
    };
};

#endif // APIBASE_H
