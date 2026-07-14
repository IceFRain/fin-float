#ifndef APIPOLLER_H
#define APIPOLLER_H

#include <QObject>
#include <QTimer>
#include "share_def.h"
#include "api_base.h"
#include "api_east.h"
#include "api_sina.h"

//API控制类
class ApiPoller : public QObject
{
    Q_OBJECT
public:
    explicit ApiPoller(QObject *parent = nullptr);
    ~ApiPoller();
    //使用设置项进行配置
    void set_by_settings(AllSettings *settings);

public slots:
    //定时器超时槽
    void slot_timer_timeout(void);
    //网络管理处理完毕槽
    void slot_net_access_finished(QNetworkReply *reply);

signals:
    //数据刷新信号
    void sig_refresh_info(ShowTargetInfo info);

private:
    //定时器
    QTimer m_timer;
    //缓存设置指针
    AllSettings *m_settings;
    //网络管理
    QNetworkAccessManager *m_net_access_manager;
    //API管理
    ApiBase *m_apis[API_TOTAL_NUM];

    //缓存当前获取到的序号
    int32_t m_current_id;
    //缓存获取数据
    APIGetInfo m_get_info;
};

#endif // APIPOLLER_H
