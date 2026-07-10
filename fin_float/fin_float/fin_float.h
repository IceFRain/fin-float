#ifndef FINFLOAT_H
#define FINFLOAT_H

#include <QWidget>
#include <QDebug>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class FinFloat; }
QT_END_NAMESPACE

class FinFloat : public QWidget
{
    Q_OBJECT

public:
    FinFloat(QWidget *parent = nullptr);
    ~FinFloat();

signals:
    //双击事件
    void sig_double_clicked(void);

protected:
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
};
#endif // FINFLOAT_H
