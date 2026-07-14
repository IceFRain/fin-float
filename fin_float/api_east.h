#ifndef APIEAST_H
#define APIEAST_H

#include <QObject>
#include "share_def.h"
#include "api_base.h"

class ApiEast : public ApiBase
{
    Q_OBJECT
public:
    ApiEast();

    //获取当前信息
    int get_current_info(APIGetInfo &get_info, QNetworkAccessManager *manager) override;
    //接收数据处理
    int handle_reply(APIGetInfo &get_info, QNetworkReply *reply) override;
};

#endif // APIEAST_H
