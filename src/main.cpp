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

    // temp style until we do "real" styling at some point
    a.setStyle("Fusion");

    QIcon folderIcon =
        QIcon::fromTheme("folder", QIcon(":/icons/folder.png"));

    MainWindow w;
    w.show();
    return a.exec();
}
