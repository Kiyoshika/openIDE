#include "MainWindow.hpp"

#include <QApplication>
#include <QMenuBar>
#include <QGridLayout>
#include <QTreeView>
#include <QIcon>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QIcon folderIcon =
        QIcon::fromTheme("folder", QIcon(":/icons/folder.png"));

    MainWindow w;
    w.show();
    return a.exec();
}
