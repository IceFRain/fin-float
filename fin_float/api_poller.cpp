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
    m_timer.stop();
    m_timer.disconnect();
    m_net_access_manager->disconnect();
    delete m_net_access_manager;
    delete m_apis[API_EAST];
    delete m_apis[API_SINA];
}

/**
  * @brief 使用设置项进行配置
  * @param settings 设置指针
  * @retval 无
  * 	@arg
 */
void ApiPoller::set_by_settings(AllSettings *settings)
{
    //缓存设置指针
    m_settings = settings;
    //定时设置为0表示手动读取
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
    //步骤置为最大
    m_get_info.step_flag = INT32_MAX;

    //获取第一个标的的数据
    m_current_id = 0;
    get_target_info();
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
        //调用API处理收包的接口
        result = m_apis[m_settings->show.api]->handle_reply(m_get_info,reply);
        //接口返回成功时进行检查,否则直接停止查询
        if(result == 0)
        {
            //有剩余待完成的步骤则继续读取本标的
            if(m_get_info.step_flag)
            {
                m_apis[m_settings->show.api]->get_current_info(m_get_info,m_net_access_manager);
                return;
            }

            //发送读取完的数据
            emit sig_refresh_info(m_get_info.info);

            //获取下一个标的的数据
            m_current_id++;
            get_target_info();
        }
    }
}

/**
  * @brief 获取标的数据
  * @param 无
  * @retval 无
  * 	@arg
 */
void ApiPoller::get_target_info()
{
    //步骤置位为最大,查找下一个要读取的标的
    m_get_info.step_flag = INT32_MAX;
    for(; m_current_id<m_settings->target.size(); m_current_id++)
    {
        //使能且代码有效才算找到
        if(m_settings->target[m_current_id].enabled
              && m_settings->target[m_current_id].code.size()>3)
        {
            break;
        }
    }
    //找到了才去操作api进行读取
    if(m_current_id<m_settings->target.size())
    {
        //读取时使用找到的代码
        m_get_info.info.code = m_settings->target[m_current_id].code;
        if(m_apis[m_settings->show.api])
        {
            m_apis[m_settings->show.api]->get_current_info(m_get_info,m_net_access_manager);
        }
    }
}
