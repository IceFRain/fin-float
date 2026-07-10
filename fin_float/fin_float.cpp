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

    // 1. 关键：开启鼠标追踪！否则不按住左键时，mouseMoveEvent不会触发
    this->setMouseTracking(true);

    this->resize(300, 400);
    loadConfigAndInit();
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

void FinFloat::loadConfigAndInit()
{
    maxPrice = 110.0;
    minPrice = 90.0;

    m_lines.clear();
    // 示例数据：名称、价格、颜色、线宽、是否横线、X偏移、线长
    m_lines.append({"左侧昨收", 100.0, QColor(0, 0, 0),       4, true, 30,  80, QRect()});
    m_lines.append({"左当前价", 108.0, QColor(235, 30, 30),   4, true, 30,  80, QRect()});
    m_lines.append({"右侧昨收", 105.0, QColor(0, 0, 0),       4, true, 180, 80, QRect()});
    m_lines.append({"右当前价", 93.0,  QColor(30, 180, 30),   4, true, 180, 80, QRect()});

    update();
}

void FinFloat::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制半透明底色背景
    painter.fillRect(rect(), QColor(10, 10, 10, 5));

    if (maxPrice <= minPrice) return;

    int margin = 30;
    int availableHeight = height() - 2 * margin;

    // 分割左右视窗的粉色中央线
    QPen midPen(QColor(255, 182, 193), 6);
    painter.setPen(midPen);
    painter.drawLine(width() / 2, margin, width() / 2, height() - margin);

    // 遍历绘制配置好的线条
    for (int i = 0; i < m_lines.size(); ++i) {
        PriceLine &line = m_lines[i];

        QPen pen(line.color);
        pen.setWidth(line.lineWidth);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);

        // 价格映射到 Y 轴像素
        int y = margin + (1.0 - (line.price - minPrice) / (maxPrice - minPrice)) * availableHeight;

        if (line.isHorizontal) {
            painter.drawLine(line.xOffset, y, line.xOffset + line.length, y);

            // 重要：计算当前线条的物理矩形，因为线可能很细(比如4像素)，鼠标很难点中，
            // 我们可以故意把它的判定高度上下放大一点（比如上下各扩展4像素，变成10像素高）
            int padding = 4;
            line.hitRect = QRect(line.xOffset, y - padding - line.lineWidth/2,
                                 line.length, line.lineWidth + padding * 2);
        }
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
  * @brief 鼠标移动事件
  * @param event 事件
  * @retval 无
  * 	@arg
 */
void FinFloat::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        // 逻辑A：按住左键移动 -> 拖动整个悬浮窗
        move(event->globalPos() - m_drag_position);
        event->accept();
    }
    else
    {
        // 逻辑B：未按下任何键移动 -> 悬停碰撞检测
        bool hitAnything = false;

        for (const PriceLine &line : m_lines) {
            // 判断当前鼠标的坐标 event->pos() 是否落在线条的判定区域内
            if (line.hitRect.contains(event->pos())) {
                // 组装你需要提示的信息文本
                QString tipStr = QString("<b>%1</b><br>价格: %2")
                                 .arg(line.name)
                                 .arg(line.price);

                // 在鼠标当前全局坐标处弹出 ToolTip
                QToolTip::showText(event->globalPos(), tipStr, this);
                hitAnything = true;
                break; // 碰到了其中一根线就跳出循环
            }
        }

        // 如果鼠标离开了所有线条区域，主动关闭当前提示
        if (!hitAnything) {
            QToolTip::hideText();
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
    //左键双击打开设置界面
    if (event->button() == Qt::LeftButton)
    {
        m_settings_w.showNormal();
        m_settings_w.raise();
        m_settings_w.activateWindow();
        event->accept();
    }
}

