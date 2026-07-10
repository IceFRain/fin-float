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
  * @brief 鼠标移动事件
  * @param event 事件
  * @retval 无
  * 	@arg
 */
void FinFloat::mouseMoveEvent(QMouseEvent *event)
{
    //鼠标点击后移动,窗口对应移动
    if (event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - m_drag_position);
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
    //鼠标双击发射信号
    if (event->button() == Qt::LeftButton)
    {
        emit sig_double_clicked();
        event->accept();
    }
}

