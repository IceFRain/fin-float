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

//显示设置项结构体
typedef struct
{
    short api;              //数据源选择
    short orientation;      //显示方向
    short mode;
    short line_width;
}ShowSettings;
//标的结构体
typedef struct
{
    short enabled;
    QString code;
    QString name;
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
    //项目官网按钮点击槽
    void slot_pb_project_url_gitee_clicked(void);
    void slot_pb_project_url_github_clicked(void);
    //设置初始化按钮点击槽
    void slot_pb_reset_settings_clicked(void);

signals:
    //设置改变信号
    void sig_settings_changed(void);

private:
    Ui::SettingsWindow *ui;
    //保存设置项
    AllSettings m_settings;

    //界面按配置初始化
    void init_ui_by_settings(void);
};

#endif // SETTINGS_WINDOW_H
