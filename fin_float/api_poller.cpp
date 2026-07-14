#include "api_poller.h"

/**
  * @brief 构造函数
  * @param parent:父窗口
  * @retval 无
  * 	@arg
 */
ApiPoller::ApiPoller(QObject *parent) : QObject(parent)
{
    //初始化变量
    m_net_access_manager = new QNetworkAccessManager(this);
    m_apis[API_SINA] = new ApiSina();
    m_apis[API_EAST] = new ApiEast();

    //连接定时器
    connect(&m_timer,&QTimer::timeout,this,&ApiPoller::slot_timer_timeout);
    //连接网络管理处理完毕槽
    connect(m_net_access_manager, &QNetworkAccessManager::finished, this, &ApiPoller::slot_net_access_finished);
}

/**
  * @brief 析构函数
  * @param 无
  * @retval 无
  * 	@arg
 */
ApiPoller::~ApiPoller()
{
    delete m_net_access_manager;
    delete m_apis[API_EAST];
    delete m_apis[API_SINA];
}

/**
  * @brief 使用设置项进行配置
  * @param 无
  * @retval 无
  * 	@arg
 */
void ApiPoller::set_by_settings(AllSettings *settings)
{
    m_settings = settings;
    //定时0则不循环读取
    if(m_settings->show.update_interval>0)
    {
        m_timer.setInterval(m_settings->show.update_interval * 60000);
        m_timer.start();
    }
    else
    {
        m_timer.stop();
    }
}

/**
  * @brief 定时器超时槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void ApiPoller::slot_timer_timeout()
{
    //步骤置位为最大
    m_get_info.step_flag = INT32_MAX;
    //查找第一个需要读取的标的,检测代码长度,避免误操作
    m_current_id = 0;
    for(; m_current_id<m_settings->target.size(); m_current_id++)
    {
        if(m_settings->target[m_current_id].enabled
                && m_settings->target[m_current_id].code.size()>3)
        {
            break;
        }
    }

    //找到了才读取
    if(m_current_id<m_settings->target.size())
    {
        m_get_info.info.code = m_settings->target[m_current_id].code;
        if(m_apis[m_settings->show.api])
        {
            m_apis[m_settings->show.api]->get_current_info(m_get_info,m_net_access_manager);
        }
    }
}

/**
  * @brief 网络管理处理完毕槽
  * @param reply 接口返回
  * @retval 无
  * 	@arg
 */
void ApiPoller::slot_net_access_finished(QNetworkReply *reply)
{
    int result = 0;

    //判断API接口返回是否正常
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug()<<"reply err:"<<reply->errorString();
        reply->deleteLater();
        qDebug()<<"API响应异常";
    }
    else
    {
        //调用API处理接口
        result = m_apis[m_settings->show.api]->handle_reply(m_get_info,reply);
        //接口返回成功时进行检查,否则直接进行确定是否需要发送信号
        if(result == 0)
        {
            if(m_get_info.step_flag)
            {
                //没有读取完毕则再继续读取本标的
                m_apis[m_settings->show.api]->get_current_info(m_get_info,m_net_access_manager);
                return;
            }

            //发送读取完的数据
            emit sig_refresh_info(m_get_info.info);

            //步骤置位为最大,查找下一个要读取的标的
            m_get_info.step_flag = INT32_MAX;
            m_current_id++;
            for(; m_current_id<m_settings->target.size(); m_current_id++)
            {
                if(m_settings->target[m_current_id].enabled
                      && m_settings->target[m_current_id].code.size()>3)
                {
                    break;
                }
            }
            //找到了才读取
            if(m_current_id<m_settings->target.size())
            {
                m_get_info.info.code = m_settings->target[m_current_id].code;
                if(m_apis[m_settings->show.api])
                {
                    m_apis[m_settings->show.api]->get_current_info(m_get_info,m_net_access_manager);
                }
            }
        }
    }
}
