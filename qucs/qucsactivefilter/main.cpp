#include <QApplication>
#include "qucsactivefilter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QucsActiveFilter w;
    w.show();
    
    return a.exec();
}
