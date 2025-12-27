#include "MainWindow.hpp"

#include <QApplication>
#include <QMenuBar>
#include <QGridLayout>
#include <QTreeView>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.initLayout();
    w.initProjectTree();

    // setup menu bar
    QMenuBar* menuBar = w.menuBar();
    w.initFileMenu(menuBar);

    w.show();
    return a.exec();
}
