#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <QWidget>
#include <QMessageBox>
#include <QDate>
#include <QFontDatabase>
#include <QComboBox>
#include <QCloseEvent>
#include "share_def.h"

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
    //设置指定序号标的名称
    void set_name(uint32_t index);

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
    //显示模式改变槽
    void slot_cb_show_mode_changed(int current_index);

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
