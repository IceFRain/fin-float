#ifndef SHARE_DEF_H
#define SHARE_DEF_H

#include <QDebug>
#include <QString>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>

//API类型
enum DATA_API{API_SINA,API_EAST,API_TOTAL_NUM};
//显示横纵向
enum SHOW_ORIENTATION{ORIENTATION_H,ORIENTATION_V};
//显示模式
enum SHOW_MODE{MODE_GRAPHICAL,MODE_NUMERIC};

//显示设置项结构体
typedef struct
{
    short api;                          //数据源选择 east,sina
    short orientation;                  //显示方向 横向,纵向
    short mode;                         //显示模式 图形,数字
    short line_width;                   //图形线宽
    short show_on_hover;                //悬停显示开关
    QString font_family;                //数字字体
    short font_point_size;              //数字字体大小
    short block_width;                  //显示块宽度
    short block_height;                 //显示块高度
    int update_interval;                //刷新间隔
}ShowSettings;
//标的结构体
typedef struct
{
    short enabled;                      //当前标的是否需要显示
    QString code;                       //当前标的代码
    QString tips;                       //当前标的名称,自动获取
}TargetInfo;
//设置项结构体
typedef struct
{
    ShowSettings show;
    QVector<TargetInfo> target;
    int show_target_count;              //要显示的标的数量,不保存,读写时计算
}AllSettings;

#define SOFT_VERSION    "V0.0.1"

//定义要悬停显示的标的信息
typedef struct{
    QString code;           //代码
    QString name;           //名称
    double prev_close;      //昨收
    double today_high;      //今高
    double today_low;       //今低
    double current_price;   //当前
}ShowTargetInfo;

#endif // SHARE_DEF_H
