#include "api_east.h"
#include <QDate>

ApiEast::ApiEast()
{

}

/**
  * @brief 接收数据处理
  * @param get_info 获取信息
  * @param reply 接收数据
  * @retval 执行状态
  * 	@arg 0正常
  * 	@arg -1不支持
  * 	@arg <1其他异常
 */
int ApiEast::handle_reply(APIGetInfo &get_info, QNetworkReply *reply)
{
    int result = 0;

    //判断返回的JSON是否合法
    QByteArray response = reply->readAll();
//    //调试输出原始数据
//    qDebug()<<"Raw response:"<<response;
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(response, &parseError);

    //本api一次即可完成所需数据获取,剩余步骤置0
    get_info.step_flag = 0;

    //JSON格式检查
    if (parseError.error != QJsonParseError::NoError)
    {
        qDebug()<<"JSON parse error:"<<parseError.errorString();
        reply->deleteLater();
        result = -3;
    }
    else
    {
        //解析json
        if (doc.isObject())
        {
            QJsonObject obj = doc.object();
            QJsonObject dataObj = obj["data"].toObject();
            QString code = get_info.info.code;
            //中证
            if(code.at(0).isDigit()&& code.size()==6)
            {
                switch (code[0].toLatin1())
                {
                    //SZ
                    case '0':
                    case '3':
                    //SH
                    case '6':
                        //个股/100得到实际价
                        get_info.info.current_price = dataObj["f43"].toInt()/100.0;
                        get_info.info.today_high = dataObj["f44"].toInt()/100.0;
                        get_info.info.today_low = dataObj["f45"].toInt()/100.0;
                        get_info.info.prev_close = dataObj["f60"].toInt()/100.0;
                    break;
                    //SZ_ETF
                    case '1':
                    //SH_ETF
                    case '5':
                        //ETF/1000得到实际价
                        get_info.info.current_price = dataObj["f43"].toInt()/1000.0;
                        get_info.info.today_high = dataObj["f44"].toInt()/1000.0;
                        get_info.info.today_low = dataObj["f45"].toInt()/1000.0;
                        get_info.info.prev_close = dataObj["f60"].toInt()/1000.0;
                    break;
                }
            }
            else
            {
                //值/1000得到实际价
                get_info.info.current_price = dataObj["f43"].toInt()/1000.0;
                get_info.info.today_high = dataObj["f44"].toInt()/1000.0;
                get_info.info.today_low = dataObj["f45"].toInt()/1000.0;
                get_info.info.prev_close = dataObj["f60"].toInt()/1000.0;
            }
            //保存名称,如果是ETF/LOF,删除从ETF/LOF开始的字样
            get_info.info.name = dataObj["f58"].toString();
            int index = get_info.info.name.indexOf("ETF");
            if (index != -1)
            {
                get_info.info.name = get_info.info.name.left(index);
            }
            index = get_info.info.name.indexOf("LOF");
            if (index != -1)
            {
                get_info.info.name = get_info.info.name.left(index);
            }
        }
        else
        {
            result = -4;
        }
    }

    return result;
}

/**
  * @brief 发送获取当前信息的包
  * @param get_info 获取信息
  * @param manager 网络管理类
  * @retval 执行状态
  * 	@arg 0正常
  * 	@arg -1不支持
  * 	@arg <1其他异常
 */
int ApiEast::get_current_info(APIGetInfo &get_info, QNetworkAccessManager *manager)
{
    QString code = get_info.info.code;
    QString url_str = "http://push2.eastmoney.com/api/qt/stock/get?secid=";

    //首位数字为港/中
    if(code.at(0).isDigit())
    {
        //5位数字为港,6位为中
        if(code.size()==5)
        {
            url_str = url_str + "116." + code ;
        }
        else
        {
            switch(code[0].toLatin1())
            {
                //SZ
                case '0':
                case '3':
                //SZ_ETF
                case '1':
                    url_str = url_str + "0." + code ;
                break;

                //SH
                case '6':
                //SH_ETF
                case '5':
                    url_str = url_str + "1." + code ;
                break;
            }
        }
    }
    else
    {
        url_str = url_str + "105." + code;
    }
    url_str = url_str + "&fields=f43,f44,f45,f58,f60";

    QUrl url(url_str);
    QNetworkRequest request(url);
    manager->get(request);
    return 0;

/*****************fields****************
f43	最新价 个股/100,etf/1000
f44	最高价
f45	最低价
f58	名称
f60	昨收价
f57	代码
f59	市场类型	1沪,0深,116港股,105美股,100指数
f46	开盘价
f71	当日均价
f47	成交量 单位手
f48	成交额 单位元
f92	换手率 %
f49	委买总量
f50	委卖总量
f51	委买总额
f52	委卖总额
f31/f32 买一价/买一量
f33/f34 买二价/买二量
f35/f36 买三价/买三量
f37/f38 买四价/买四量
f39/f40 买五价/买五量
f19/f20 卖一价/卖一量
f17/f18 卖二价/卖二量
f15/f16 卖三价/卖三量
f13/f14 卖四价/卖四量
f11/f12 卖五价/卖五量
f84	主力净流入 单位元
f85	主力净流出 单位元
f86	主力资金总额 单位元
f80	交易时间段
f103 超大单净流入 单位元
f104 大单净流入 单位元
f105 中单净流入 单位元
f108 资金流向比率
f116 总市值 单位元
f117 流通市值 单位元
f62 涨跌状态
f55 流动负债
f78 中单净额
f106 涨停价
f107 跌停价
f109 小单净额
f110 小单净占比
f111 委比
f112 委差
f113 外盘
f114 内盘
f115 成交笔数
***************************************/
}
