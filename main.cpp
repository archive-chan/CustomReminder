#include "CustomReminder.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CustomReminder w;
    w.show();
    return a.exec();
}
