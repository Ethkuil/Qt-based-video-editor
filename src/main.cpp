#include "QtBasedVideoEditor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtBasedVideoEditor w;
    w.show();
    return a.exec();
}