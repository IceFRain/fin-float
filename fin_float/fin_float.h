#ifndef FINFLOAT_H
#define FINFLOAT_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>
#include <QMenu>
#include <QMutex>
#include "share_def.h"
#include "settings_window.h"
#include "api_poller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FinFloat; }
QT_END_NAMESPACE

//单线条的信息
typedef struct{
    QColor color;           //线条颜色
    QPoint start_point;     //起始点
    QPoint end_point;       //结束点
    int line_width;         //线宽
}LineInfo;
//线条组
typedef struct{
    QVector<LineInfo> lines;
}LineGroup;

class FinFloat : public QWidget
{
    Q_OBJECT

public:
    FinFloat(QWidget *parent = nullptr);
    ~FinFloat();

public slots:
    //刷新显示槽
    void slot_refresh_info(ShowTargetInfo info);
    //设置改变槽
    void slot_settings_changed(void);

signals:
    //双击事件
    void sig_double_clicked(void);

//重写窗口事件
protected:
    //绘制事件
    void paintEvent(QPaintEvent *event) override;
    //鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override;
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;
    //鼠标双击事件
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    //鼠标右键事件
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    Ui::FinFloat *ui;
    //记录鼠标位置,移动悬浮窗使用
    QPoint m_drag_position{0,0};
    //设置界面
    SettingsWindow m_settings_w;
    //设置项
    AllSettings *m_settings;
    //API接口
    ApiPoller m_api_poller;

    //要显示的所有线条
    QVector<LineGroup> m_line_group;
    //缓存最新的数据
    QVector<ShowTargetInfo> m_target_infos;
//    //保护m_target_infos和m_line_group
//    QMutex m_data_mutex;


    //使用设置项初始化界面
    void init_ui_by_settings(void);
};
#endif // FINFLOAT_H
