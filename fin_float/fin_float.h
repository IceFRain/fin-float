#ifndef FINFLOAT_H
#define FINFLOAT_H

#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QVector>
#include <QPainter>
#include <QToolTip> // 新增：用于显示气泡提示
#include "settings_window.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FinFloat; }
QT_END_NAMESPACE

// 定义单条价格线的数据结构
struct PriceLine {
    QString name;       // 线条名称，如 "昨收", "当前价", "阻力位"
    double price;       // 价格值
    QColor color;       // 线条颜色
    int lineWidth;      // 线条粗细
    bool isHorizontal;  // true为横线，false为纵线
    int xOffset;        // 横向起点像素
    int length;         // 线条长度/宽度（像素）

    // 用于碰撞检测的缓存区域（在paintEvent中动态计算）
    QRect hitRect;
};

class FinFloat : public QWidget
{
    Q_OBJECT

public:
    FinFloat(QWidget *parent = nullptr);
    ~FinFloat();

    void loadConfigAndInit(); // 新增：初始化数据

signals:
    //双击事件
    void sig_double_clicked(void);

protected:
    // 新增：绘制事件
    void paintEvent(QPaintEvent *event) override;
    //鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override;
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;
    //鼠标双击事件
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    Ui::FinFloat *ui;
    //记录鼠标位置,移动悬浮窗使用
    QPoint m_drag_position{0,0};
    //设置界面
    SettingsWindow m_settings_w;

    // 新增：数据成员
    QVector<PriceLine> m_lines;
    double maxPrice{110.0};
    double minPrice{90.0};
};
#endif // FINFLOAT_H
