#include "fin_float.h"
#include "ui_fin_float.h"

/**
  * @brief 构造函数
  * @param parent:父窗口
  * @retval 无
  * 	@arg
 */
FinFloat::FinFloat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FinFloat)
{
    ui->setupUi(this);

    //设置窗口标志 工具窗口 无标题栏 顶层显示
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //背景透明
    this->setAttribute(Qt::WA_TranslucentBackground);

    //开启鼠标追踪,用于悬停显示
    this->setMouseTracking(true);

    //连接设置改变槽
    connect(&m_settings_w,&SettingsWindow::sig_settings_changed,this,&FinFloat::slot_settings_changed);
    //连接刷新槽
    connect(&m_api_poller,&ApiPoller::sig_refresh_info,this,&FinFloat::slot_refresh_info);

    //用设置项初始化ui
    m_settings = m_settings_w.get_settings_pointer();
    init_ui_by_settings();
    //用设置项初始化api
    m_api_poller.set_by_settings(m_settings_w.get_settings_pointer());
    //立即刷新一次
    m_api_poller.slot_timer_timeout();
}

/**
  * @brief 析构函数
  * @param 无
  * @retval 无
  * 	@arg
 */
FinFloat::~FinFloat()
{
    delete ui;
}

/**
  * @brief 刷新显示槽
  * @param info 待刷新的数据
  * @retval 无
  * 	@arg
 */
void FinFloat::slot_refresh_info(ShowTargetInfo info)
{
    //找对应代码是第几个显示
    int32_t index = -1;
    for (int32_t i=0,j=0;i<m_settings->target.size();i++)
    {
        if(m_settings->target[i].enabled)
        {
            if(m_settings->target[i].code == info.code)
            {
                //记录显示位置
                index = j;
                break;
            }
            j++;
        }
    }
    //找到后设置对应显示组
    if(index>=0)
    {
        //缓存最新信息
        m_target_infos[index] = info;
        //清除当前标的显示后重新计算显示
        m_line_group[index].lines.clear();
        //横向显示计算位置
        if(m_settings->show.orientation == ORIENTATION_H)
        {
            //图表显示
            if(m_settings->show.mode == MODE_GRAPHICAL)
            {
                uint32_t x_start = 2 + index*m_settings->show.block_width;
                uint32_t x_end = x_start + m_settings->show.block_width - 5;
                uint32_t x_mid = (x_start + x_end)/2;
                double price_high = (info.today_high>info.prev_close)? info.today_high:info.prev_close;
                double price_low = (info.today_low<info.prev_close)? info.today_low:info.prev_close;
                LineInfo line;
                uint32_t y = 0;
                double ratio = 0.0;
                line.line_width = m_settings->show.line_width;
                line.color = Qt::darkGray;
                //停牌等异常情况处理
                if(price_high==price_low)
                {
                    y = m_settings->show.block_height/2;
                    line.start_point = QPoint(x_start,y);
                    line.end_point = QPoint(x_end,y);
                }
                else
                {
                    //昨收显示
                    ratio = (price_high - info.prev_close) / (price_high - price_low);
                    y = 1 + ratio * (m_settings->show.block_height - 3);
                    line.start_point = QPoint(x_start,y);
                    line.end_point = QPoint(x_end,y);
                    m_line_group[index].lines.push_back(line);
                    //现价显示
                    if(info.current_price > info.prev_close)
                    {
                        line.color = QColor("#F08080");
                    }
                    else
                    {
                        line.color = QColor("#90EE90");
                    }
                    ratio = (price_high - info.current_price) / (price_high - price_low);
                    y = 1 + ratio * (m_settings->show.block_height - 3);
                    line.start_point = QPoint(x_start,y);
                    line.end_point = QPoint(x_end,y);
                    m_line_group[index].lines.push_back(line);
                    //今日范围显示
                    ratio = (price_high - info.today_high) / (price_high - price_low);
                    y = 1 + ratio * (m_settings->show.block_height - 3);
                    line.start_point = QPoint(x_mid,y);
                    ratio = (price_high - info.today_low) / (price_high - price_low);
                    y = 1 + ratio * (m_settings->show.block_height - 3);
                    line.end_point = QPoint(x_mid,y);
                    m_line_group[index].lines.push_back(line);
                }
            }
            //todo 数字显示
            else
            {

            }
        }
        //todo 纵向显示
        else
        {

        }
    }

    update();
}

/**
  * @brief 设置改变槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void FinFloat::slot_settings_changed()
{
    //初始化界面后清空显示
    init_ui_by_settings();
    update();
    //同步给api
    m_api_poller.set_by_settings(m_settings_w.get_settings_pointer());
    m_api_poller.slot_timer_timeout();
}

/**
  * @brief 重写绘制事件
  * @param 无
  * @retval 无
  * 	@arg
 */
void FinFloat::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //绘制近乎透明的底色背景
    painter.fillRect(rect(), QColor(255, 255, 255, 1));
    painter.translate(0.5, 0.5);

    //图表显示
    if(m_settings->show.mode == MODE_GRAPHICAL)
    {
        for (int32_t i=0; i<m_line_group.size(); i++)
        {
            const LineGroup &group = m_line_group[i];
            for (int32_t j=0; j< group.lines.size();j++)
            {
                const LineInfo &line = m_line_group[i].lines[j];
                //设置画笔:颜色+线宽
                painter.setPen(QPen(line.color, line.line_width));
                //画线
                painter.drawLine(line.start_point, line.end_point);
            }
        }
    }
    //todo 数字显示
    else
    {

    }
}

/**
  * @brief 鼠标点击事件
  * @param event 事件
  * @retval 无
  * 	@arg
 */
void FinFloat::mousePressEvent(QMouseEvent *event)
{
    //记录鼠标点击位置
    if (event->button() == Qt::LeftButton)
    {
        m_drag_position = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

/**
  * @brief 鼠标拖动和悬停事件
  * @param event 事件
  * @retval 无
  * 	@arg
 */
void FinFloat::mouseMoveEvent(QMouseEvent *event)
{
    //鼠标拖动
    if (event->buttons() & Qt::LeftButton)
    {
        //按住左键移动 -> 拖动整个悬浮窗
        move(event->globalPos() - m_drag_position);
        event->accept();
    }
    //鼠标悬停
    else
    {
        if(m_settings->show.show_on_hover)
        {
            //横向显示计算位置
            if(m_settings->show.orientation == ORIENTATION_H)
            {
                int index = event->x() / m_settings->show.block_width;
                if(index<m_target_infos.size())
                {
                    ShowTargetInfo &info = m_target_infos[index];
                    if(!info.name.isEmpty())
                    {
                        QString show_str = info.name + "\n" + QString::number(info.current_price) + "\n";
                        double rate = info.current_price/info.prev_close;
                        if(rate > 0)
                        {
                            rate -= 1;
                        }
                        else
                        {
                            rate = 1-rate;
                        }
                        rate *= 100;
                        show_str = show_str + QString::number(rate,'f',2) + "%";
                        QToolTip::showText(event->globalPos(), show_str, this);
                    }
                    else
                    {
                        QToolTip::hideText();
                    }
                }
            }
            //todo 纵向显示
            else
            {

            }
        }
        event->accept();
    }
}

/**
  * @brief 鼠标双击事件
  * @param event 事件
  * @retval 无
  *     @arg
 */
void FinFloat::mouseDoubleClickEvent(QMouseEvent *event)
{
    //双击打开设置界面
    if (event->button() == Qt::LeftButton)
    {
        m_settings_w.showNormal();
        m_settings_w.raise();
        m_settings_w.activateWindow();
        event->accept();
    }
}

/**
  * @brief 鼠标右击事件
  * @param event 事件
  * @retval 无
  *     @arg
 */
void FinFloat::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    //添加单次刷新
    QAction *refresh_action = menu.addAction("刷新");
    connect(refresh_action,&QAction::triggered,&m_api_poller,&ApiPoller::slot_timer_timeout);
    QAction *exit_action = menu.addAction("退出程序");
    connect(exit_action, &QAction::triggered, qApp, &QApplication::quit);

    //在鼠标位置弹出
    menu.exec(event->globalPos());
}

/**
  * @brief 使用设置项初始化界面
  * @param 无
  * @retval 无
  *     @arg
 */
void FinFloat::init_ui_by_settings()
{
    ShowSettings &show = m_settings->show;

    //图表显示
    if(m_settings->show.mode == MODE_GRAPHICAL)
    {
        //最小留一个大小,避免无悬浮窗
        if(m_settings->show_target_count==0)
        {
            this->resize(show.block_width,show.block_height);
            m_line_group.resize(1);
            m_line_group[0].lines.clear();
            LineInfo line;
            line.color = Qt::gray;
            line.line_width = m_settings->show.line_width;
            line.start_point = QPoint(2,1);
            line.end_point = QPoint(m_settings->show.block_width - 3,m_settings->show.block_height-2);
            m_line_group[0].lines.append(line);
            return;
        }
    }

    //横向显示计算尺寸
    if(show.orientation == ORIENTATION_H)
    {
        this->resize(m_settings->show_target_count*show.block_width,show.block_height);
    }
    //纵向
    else
    {
        this->resize(show.block_width,m_settings->show_target_count*show.block_height);
    }

    //初始化后清除所有已划线,重新给每个标的画一根灰色斜线,避免悬浮窗隐身
    m_target_infos.resize(m_settings->show_target_count);
    //图表显示
    if(m_settings->show.mode == MODE_GRAPHICAL)
    {
        m_line_group.resize(m_settings->show_target_count);
        for (int32_t i=0;i<m_settings->show_target_count;i++)
        {
            //横向显示计算位置
            if(m_settings->show.orientation == ORIENTATION_H)
            {
                m_line_group[i].lines.clear();
                LineInfo line;
                line.color = Qt::gray;
                line.line_width = m_settings->show.line_width;
                line.start_point = QPoint(2 + i*m_settings->show.block_width,1);
                line.end_point = QPoint(i*m_settings->show.block_width + m_settings->show.block_width - 3,m_settings->show.block_height-2);
                m_line_group[i].lines.append(line);
            }
            //todo 纵向显示
            else
            {

            }
        }
    }
    //todo 数字显示
    else
    {

    }
    //刷新显示
    update();
}




