#include "fin_float.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FinFloat w;
    w.show();
    return a.exec();
}
