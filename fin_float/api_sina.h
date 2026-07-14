#ifndef APISINA_H
#define APISINA_H

#include <QObject>
#include <QTextCodec>
#include "share_def.h"
#include "api_base.h"


class ApiSina : public ApiBase
{
    Q_OBJECT
public:
    ApiSina();

    //获取当前信息
    int get_current_info(APIGetInfo &get_info, QNetworkAccessManager *manager) override;
    //接收数据处理
    int handle_reply(APIGetInfo &get_info, QNetworkReply *reply) override;
};

#endif // APISINA_H
