#include <QApplication>
#include "filtersintez.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FilterSintez w;
    w.show();
    
    return a.exec();
}
