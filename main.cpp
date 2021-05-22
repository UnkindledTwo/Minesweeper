#include "minesweeper.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Minesweeper w;
    QApplication::setStyle(QStyleFactory::create("gtk"));
    w.show();
    return a.exec();
}
