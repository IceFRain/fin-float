#include "settings_window.h"
#include "ui_settings_window.h"

/**
  * @brief 构造函数
  * @param parent:父窗口
  * @retval 无
  * 	@arg
 */
SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    //设置窗口名
    setWindowTitle("设置");

    //设置列表文字居中
    for (int i = 0; i < ui->listWidget_setting_group->count(); i++)
    {
        ui->listWidget_setting_group->item(i)->setTextAlignment(Qt::AlignCenter);
    }
    //设置版本号
    ui->label_version->setText(SOFT_VERSION);
    QString bt = QString(__DATE__ " " __TIME__).simplified();
    QDateTime dt = QLocale(QLocale::English).toDateTime(bt, "MMM d yyyy HH:mm:ss");
    ui->label_build_time->setText(dt.toString("yyyy.MM.dd HH:mm:ss"));

    //设置列表选择和页面切换连接
    connect(ui->listWidget_setting_group,&QListWidget::currentRowChanged,ui->stackedWidget_settings,&QStackedWidget::setCurrentIndex);

    //设置按钮连接
    connect(ui->pushButton_show_change_cancel,&QPushButton::clicked,this,&SettingsWindow::slot_pb_change_cancel_clicked);
    connect(ui->pushButton_show_change_save,&QPushButton::clicked,this,&SettingsWindow::slot_pb_change_save_clicked);
    connect(ui->pushButton_target_add,&QPushButton::clicked,this,&SettingsWindow::slot_pb_target_add_clicked);
    connect(ui->pushButton_target_del,&QPushButton::clicked,this,&SettingsWindow::slot_pb_target_del_clicked);
    connect(ui->pushButton_project_url_github,&QPushButton::clicked,this,&SettingsWindow::slot_pb_project_url_github_clicked);
    connect(ui->pushButton_project_url_gitee,&QPushButton::clicked,this,&SettingsWindow::slot_pb_project_url_gitee_clicked);
    connect(ui->pushButton_reset_settings,&QPushButton::clicked,this,&SettingsWindow::slot_pb_reset_settings_clicked);

    //默认选择第一组设置项
    ui->listWidget_setting_group->setCurrentRow(0);
    //设置表头
    ui->treeWidget_target->setHeaderLabels(QStringList() << "显示" << "代码" << "名称");
    ui->treeWidget_target->headerItem()->setTextAlignment(0, Qt::AlignCenter);
    ui->treeWidget_target->header()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->treeWidget_target->headerItem()->setTextAlignment(1, Qt::AlignCenter);
    ui->treeWidget_target->headerItem()->setTextAlignment(2, Qt::AlignCenter);
    ui->treeWidget_target->setColumnWidth(0, 58);
    ui->treeWidget_target->setColumnWidth(1, 150);
    ui->treeWidget_target->header()->setStretchLastSection(true);  // 最后一列自动填满

    //读取配置文件
    read_settings_from_file();
}

/**
  * @brief 析构函数
  * @param 无
  * @retval 无
  * 	@arg
 */
SettingsWindow::~SettingsWindow()
{
    delete ui;
}

/**
  * @brief 获取设置项指针
  * @param 无
  * @retval 设置项指针
  * 	@arg
 */
AllSettings *SettingsWindow::get_settings_pointer()
{
    return &m_settings;
}

/**
  * @brief 从文件中读取设置
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::read_settings_from_file()
{
    QFile file("settings.json");
    QJsonObject root;
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "settings.json open failed, use default settings";
    }
    else
    {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
        file.close();

        if (error.error == QJsonParseError::NoError)
        {
            root = doc.object();
        }
        else
        {
            qDebug() << "JSON parse failed:" << error.errorString()<<", use default settings";
        }
    }

    QJsonObject show  = root["show"].toObject();
    m_settings.show.api = show["api"].toInt(0);
    m_settings.show.orientation = show["orientation"].toInt(0);
    m_settings.show.mode = show["mode"].toInt(0);
    m_settings.show.line_width = show["line_width"].toInt(1);
    m_settings.show.show_on_hover = show["show_on_hover"].toInt(1);
    m_settings.show.font_family = show["font_family"].toString("MiSans");
    m_settings.show.font_point_size = show["font_point_size"].toInt(12);
    m_settings.show.block_width = show["block_width"].toInt(10);
    m_settings.show.block_height = show["block_height"].toInt(20);

    m_settings.target.clear();
    QJsonArray targetArray = root["target"].toArray();
    for (const QJsonValue &val : targetArray)
    {
        QJsonObject obj = val.toObject();
        TargetInfo info;
        info.enabled  = obj["enabled"].toInt(0);
        info.code     = obj["code"].toString();
        info.name     = obj["name"].toString();
        m_settings.target.append(info);
    }

    init_ui_by_settings();
}

/**
  * @brief 写入设置到文件
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::save_settings_to_file()
{
    //从界面读取当前的配置
    m_settings.show.api = ui->comboBox_show_api->currentIndex();
    m_settings.show.orientation = ui->comboBox_show_orientation->currentIndex();
    m_settings.show.mode = ui->comboBox_show_mode->currentIndex();
    m_settings.show.line_width = ui->spinBox_show_line_width->value();
    m_settings.show.show_on_hover = ui->checkBox_show_on_hover->isChecked();
    m_settings.show.font_family = ui->comboBox_font_family->currentText();
    m_settings.show.font_point_size = ui->comboBox_font_point_size->currentText().toInt();
    m_settings.show.block_width = ui->spinBox_show_block_width->value();
    m_settings.show.block_height = ui->spinBox_show_block_height->value();
    //读取标的列表
    m_settings.target.clear();
    for (int i = 0; i < ui->treeWidget_target->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->treeWidget_target->topLevelItem(i);
        TargetInfo info;
        info.enabled = (item->checkState(0) == Qt::Checked) ? 1 : 0;
        info.code    = item->text(1);
        info.name    = item->text(2);
        m_settings.target.append(info);
    }

    //show 部分
    QJsonObject show;
    show["api"]            = m_settings.show.api;
    show["orientation"]    = m_settings.show.orientation;
    show["mode"]           = m_settings.show.mode;
    show["line_width"]     = m_settings.show.line_width;
    show["show_on_hover"]  = m_settings.show.show_on_hover;
    show["font_family"]    = m_settings.show.font_family;
    show["font_point_size"]= m_settings.show.font_point_size;
    show["block_width"]    = m_settings.show.block_width;
    show["block_height"]   = m_settings.show.block_height;

    //target 部分
    QJsonArray targetArray;
    for (const TargetInfo &info : m_settings.target)
    {
        QJsonObject obj;
        obj["enabled"] = info.enabled;
        obj["code"]    = info.code;
        obj["name"]    = info.name;
        targetArray.append(obj);
    }

    //组装根对象
    QJsonObject root;
    root["show"]   = show;
    root["target"] = targetArray;

    //写文件
    QFile file("settings.json");
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "settings.json save failed";
        return;
    }
    file.write(QJsonDocument(root).toJson());
    file.close();
}

/**
  * @brief 设置保存按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::slot_pb_change_save_clicked()
{
    //二次确认
    QMessageBox::StandardButton btn = QMessageBox::question(
        this, "确认", "要保存更改吗？");

    if (btn == QMessageBox::Yes)
    {
        save_settings_to_file();
    }
}

/**
  * @brief 设置取消保存按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::slot_pb_change_cancel_clicked()
{
    //二次确认
    QMessageBox::StandardButton btn = QMessageBox::question(
        this, "确认", "要撤销所有更改吗？");

    if (btn == QMessageBox::Yes)
    {
        read_settings_from_file();
    }
}

/**
  * @brief 增加标的点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::slot_pb_target_add_clicked()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_target);
    item->setCheckState(0,Qt::Checked);
    item->setText(1,"000000");
    item->setText(2,"");
    item->setTextAlignment(1, Qt::AlignCenter);
    item->setTextAlignment(2, Qt::AlignCenter);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}

/**
  * @brief 删除标的点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::slot_pb_target_del_clicked()
{
    QTreeWidgetItem *item = ui->treeWidget_target->currentItem();
    if (item)
    {
        delete item;
    }
}

/**
  * @brief 项目gitee源点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::slot_pb_project_url_gitee_clicked()
{
    QDesktopServices::openUrl(QUrl("https://gitee.com/IceFRain/fin-float"));
}

/**
  * @brief 项目github源点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::slot_pb_project_url_github_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/IceFRain/fin-float"));
}

/**
  * @brief 设置初始化按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::slot_pb_reset_settings_clicked()
{
    //二次确认
    QMessageBox::StandardButton btn = QMessageBox::question(
        this, "确认", "要重置所有设置吗？");

    if (btn == QMessageBox::Yes)
    {
        //删除当前设置文件后,重新读取数据,设置为默认值
        QFile::remove("./settings.json");
        read_settings_from_file();
        emit sig_settings_changed();
    }
}

/**
  * @brief 重写关闭事件
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::closeEvent(QCloseEvent *event)
{
    uint8_t changed_flag=0;
    //先比较标的列表
    if(ui->treeWidget_target->topLevelItemCount() != m_settings.target.size())
    {
        changed_flag = 1;
    }
    else
    {
        for (int i = 0; i < ui->treeWidget_target->topLevelItemCount(); i++)
        {
            QTreeWidgetItem *item = ui->treeWidget_target->topLevelItem(i);
            TargetInfo &info = m_settings.target[i];
            if(     info.enabled != ((item->checkState(0) == Qt::Checked) ? 1 : 0)
                ||  info.code != item->text(1)
                ||  info.name != item->text(2))
            {
                changed_flag = 1;
                break;
            }
        }
    }
    //再比较标的列表外的
    if(!changed_flag)
    {
        if(m_settings.show.api != ui->comboBox_show_api->currentIndex()){
            changed_flag = 1;
        }else if(m_settings.show.orientation != ui->comboBox_show_orientation->currentIndex()){
            changed_flag = 1;
        }else if(m_settings.show.mode != ui->comboBox_show_mode->currentIndex()){
            changed_flag = 1;
        }else if(m_settings.show.line_width != ui->spinBox_show_line_width->value()){
            changed_flag = 1;
        }else if(m_settings.show.show_on_hover != ui->checkBox_show_on_hover->isChecked()){
            changed_flag = 1;
        }else if(m_settings.show.font_family != ui->comboBox_font_family->currentText()){
            changed_flag = 1;
        }else if(m_settings.show.font_point_size != ui->comboBox_font_point_size->currentText().toInt()){
            changed_flag = 1;
        }else if(m_settings.show.block_width != ui->spinBox_show_block_width->value()){
            changed_flag = 1;
        }else if(m_settings.show.block_height != ui->spinBox_show_block_height->value()){
            changed_flag = 1;
        }
    }

    //存在修改则提示是否保存
    if (changed_flag)
    {
        QMessageBox::StandardButton btn = QMessageBox::question(
            this, "确认", "设置已修改,是否保存?");

        if (btn == QMessageBox::Yes)
        {
            save_settings_to_file();
            event->accept();
        }
        else
        {
            read_settings_from_file();
            event->accept();
        }
    }
    else
    {
        event->accept();
    }
}


/**
  * @brief 界面按配置初始化
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::init_ui_by_settings()
{
    //添加设置项
    ui->comboBox_show_api->clear();
    ui->comboBox_show_api->addItem("sina",API_SINA);
    ui->comboBox_show_api->addItem("east",API_EAST);
    ui->comboBox_show_orientation->clear();
    ui->comboBox_show_orientation->addItem("横向",ORIENTATION_H);
    ui->comboBox_show_orientation->addItem("纵向",ORIENTATION_V);
    ui->comboBox_show_mode->clear();
    ui->comboBox_show_mode->addItem("图形",MODE_GRAPHICAL);
    ui->comboBox_show_mode->addItem("数字",MODE_NUMERIC);
    //字体设置项
    QFontDatabase fontDatabase;
    QStringList families = fontDatabase.families();
    ui->comboBox_font_family->clear();
    ui->comboBox_font_family->addItems(families);
    QStringList sizes;
    sizes << "6" << "7" << "8" << "9" << "10" << "10.5" << "11" << "12"
          << "14" << "16" << "18" << "20" << "22" << "24" << "26" << "28";
    ui->comboBox_font_point_size->clear();
    ui->comboBox_font_point_size->addItems(sizes);

    //按读取到的配置进行初始化
    ui->comboBox_show_api->setCurrentIndex(m_settings.show.api);
    ui->comboBox_show_orientation->setCurrentIndex(m_settings.show.orientation);
    ui->comboBox_show_mode->setCurrentIndex(m_settings.show.mode);
    ui->spinBox_show_line_width->setValue(m_settings.show.line_width);
    ui->checkBox_show_on_hover->setChecked(m_settings.show.show_on_hover);
    ui->comboBox_font_family->setCurrentText(m_settings.show.font_family);
    ui->comboBox_font_point_size->setCurrentText(QString::number(m_settings.show.font_point_size));
    ui->spinBox_show_block_width->setValue(m_settings.show.block_width);
    ui->spinBox_show_block_height->setValue(m_settings.show.block_height);

    //填充标的数据
    ui->treeWidget_target->clear();
    ui->treeWidget_target->setHeaderLabels(QStringList() << "显示" << "代码" << "名称");
    for (const TargetInfo &info : m_settings.target)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_target);
        item->setCheckState(0, info.enabled ? Qt::Checked : Qt::Unchecked);
        item->setText(1, info.code);
        item->setText(2, info.name);
        item->setTextAlignment(1, Qt::AlignCenter);
        item->setTextAlignment(2, Qt::AlignCenter);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }
}
