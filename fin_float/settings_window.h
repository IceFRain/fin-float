#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QDate>
#include <QFontDatabase>
#include <QComboBox>
#include <QCloseEvent>

//API类型
enum DATA_API{API_SINA,API_EAST};
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
}ShowSettings;
//标的结构体
typedef struct
{
    short enabled;                      //当前标的是否需要显示
    QString code;                       //当前标的代码
    QString name;                       //当前标的名称,自动获取
}TargetInfo;
//设置项结构体
typedef struct
{
    ShowSettings show;
    QVector<TargetInfo> target;
}AllSettings;

#define SOFT_VERSION    "V0.0.1"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();
    //获取设置项指针,允许外部修改
    AllSettings* get_settings_pointer(void);
    //读取保存的参数配置
    void read_settings_from_file(void);
    //保存参数配置
    void save_settings_to_file(void);

public slots:
    //设置保存按钮点击槽
    void slot_pb_change_save_clicked(void);
    //设置取消保存按钮点击槽
    void slot_pb_change_cancel_clicked(void);
    //增加标的点击槽
    void slot_pb_target_add_clicked(void);
    //删除标的点击槽
    void slot_pb_target_del_clicked(void);
    //项目官网按钮点击槽
    void slot_pb_project_url_gitee_clicked(void);
    void slot_pb_project_url_github_clicked(void);
    //设置初始化按钮点击槽
    void slot_pb_reset_settings_clicked(void);

signals:
    //设置改变信号
    void sig_settings_changed(void);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::SettingsWindow *ui;
    //保存设置项
    AllSettings m_settings;

    //界面按配置初始化
    void init_ui_by_settings(void);
};

#endif // SETTINGS_WINDOW_H
