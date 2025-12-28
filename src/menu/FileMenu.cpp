#include "menu/FileMenu.hpp"
#include "ProjectTree.hpp"
#include "MainWindow.hpp"

using namespace openide;
using namespace openide::menu;

FileMenu::FileMenu(MainWindow* parent, QMenuBar* menuBar, ProjectTree* projectTree)
    : QMenu(parent ? parent->getCentralWidget() : parent)
    , m_projectTree{projectTree}
{
    if (!parent || !menuBar) return;

    QMenu* fileMenu = menuBar->addMenu("&File");
    QAction* newAction = new QAction("&New Project", this);
    QAction* openAction = new QAction("&Open Project", this);

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);

    QObject::connect(openAction, &QAction::triggered, this, [this](){
        QString filePath;
        FileMenu::openDir(this, &filePath);
        if (!filePath.isEmpty())
            m_projectTree->loadTreeFromDir(&filePath);
    });
}

void FileMenu::openFile(QWidget* parent, QString* filePath)
{
    if (!parent || !filePath) return;
    QString selectedFilePath = QFileDialog::getOpenFileName(parent, "Open File", QDir::homePath(), "All Files (*)");
    *filePath = selectedFilePath;
}

void FileMenu::openDir(QWidget* parent, QString* dirPath)
{
    if (!parent || !dirPath) return;
    QString selectedDirPath = QFileDialog::getExistingDirectory(parent, "Open Direcotry", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    *dirPath = selectedDirPath;
}
