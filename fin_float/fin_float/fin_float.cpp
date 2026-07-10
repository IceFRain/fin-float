#include "fin_float.h"
#include "ui_fin_float.h"

FinFloat::FinFloat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FinFloat)
{
    ui->setupUi(this);
}

FinFloat::~FinFloat()
{
    delete ui;
}

