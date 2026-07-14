#include "api_sina.h"

ApiSina::ApiSina()
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
int ApiSina::get_current_info(APIGetInfo &get_info, QNetworkAccessManager *manager)
{
    QString url_str = "http://hq.sinajs.cn/list=";
    QString code = get_info.info.code;
    switch(code[0].toLatin1())
    {
        //SZ
        case '0':
        case '3':
        //SZ_ETF
        case '1':
           url_str = url_str + "sz" + code;
        break;

        //SH
        case '6':
        //SH_ETF
        case '5':
           url_str = url_str + "sh" + code;
        break;
    }
    QNetworkRequest request(url_str);

    //设置Referer请求头伪装请求来源绕过403限制
    request.setRawHeader("Referer", "http://finance.sina.com.cn");
    //加上User-Agent显得更像浏览器
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    manager->get(request);

    return 0;
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
int ApiSina::handle_reply(APIGetInfo &get_info, QNetworkReply *reply)
{
    int result = 0;

    //判断返回的JSON是否合法
    QByteArray res = reply->readAll();
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString response = codec->toUnicode(res); // 转换后，resultStr 内部已经是正确的中文了
//    //调试输出原始数据
//    qDebug()<<"Raw response:"<<response;

    //本api一次即可完成所需数据获取
    get_info.step_flag = 0;

    //定位并截取双引号内的有效数据
    int firstQuote = response.indexOf('"');
    int lastQuote = response.lastIndexOf('"');
    if (firstQuote == -1 || lastQuote <= firstQuote)
    {
        return -2;
    }

    QString content = response.mid(firstQuote + 1, lastQuote - firstQuote - 1);
    //按逗号分割字符串
    QStringList fields = content.split(",");

    //提取所需数据
    if (fields.size() >= 6)
    {
        get_info.info.name = fields[0];
        get_info.info.prev_close = fields[2].toDouble();
        get_info.info.current_price = fields[3].toDouble();
        get_info.info.today_high = fields[4].toDouble();
        get_info.info.today_low = fields[5].toDouble();
    }
    //保存名称,如果是ETF/LOF,删除从ETF/LOF开始的字样
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

    return result;

/*****************fields****************
0	名称
1   今开
2	昨收
3   当前
4   今高
5   今低
6   买一
7   卖一
8   成交数
9   成交额
10  买一数
11  买一
12  买二数
13  买二
14  买三数
15  买三
16  买四数
17  买四
18  买五数
19  买五
20  卖一数
21  卖一
22  卖二数
23  卖二
24  卖三数
25  卖三
26  卖四数
27  卖四
28  卖五数
29  卖五
30  日期
31  时间
32  状态码
***************************************/
}
