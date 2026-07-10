#ifndef FINFLOAT_H
#define FINFLOAT_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class FinFloat; }
QT_END_NAMESPACE

class FinFloat : public QWidget
{
    Q_OBJECT

public:
    FinFloat(QWidget *parent = nullptr);
    ~FinFloat();

private:
    Ui::FinFloat *ui;
};
#endif // FINFLOAT_H
