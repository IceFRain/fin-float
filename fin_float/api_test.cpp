#include "api_test.h"

ApiTest::ApiTest()
{

}

int ApiTest::get_current_info(APIGetInfo &get_info, QNetworkAccessManager *manager)
{
    qDebug()<<"get_current_info";
}

int ApiTest::handle_reply(APIGetInfo &get_info, QNetworkReply *reply)
{

}
