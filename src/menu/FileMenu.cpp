#include "menu/FileMenu.hpp"
#include "ProjectTree.hpp"
#include "MainWindow.hpp"

using namespace openide;
using namespace openide::menu;

FileMenu::FileMenu(
    MainWindow* parent,
    QMenuBar* menuBar,
    ProjectTree* projectTree,
    std::function<void()> saveFileCallback,
    std::function<void()> saveAllFilesCallback)
    : QMenu(parent ? parent->getCentralWidget() : parent)
    , m_projectTree{projectTree}
    , m_saveFileCallback{saveFileCallback}
    , m_saveAllFilesCallback{saveAllFilesCallback}
{
    if (!parent || !menuBar) return;

    QMenu* fileMenu = menuBar->addMenu("&File");
    QAction* newAction = new QAction("&New Project", this);
    QAction* openAction = new QAction("&Open Project", this);
    QAction* saveAction = new QAction("&Save File", this);
    QAction* saveAllAction = new QAction("Save &All Files", this);

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAllAction);

    // new project action
    QObject::connect(newAction, &QAction::triggered, this, [this](){
        // TODO:
    });

    // open file action
    QObject::connect(openAction, &QAction::triggered, this, [this](){
        QString filePath;
        FileMenu::openDir(this, &filePath);
        if (!filePath.isEmpty())
            m_projectTree->loadTreeFromDir(&filePath);
    });

    // save file action
    QObject::connect(saveAction, &QAction::triggered, this, [this](){
        if (m_saveFileCallback) m_saveFileCallback();
    });

    // save all files action
    QObject::connect(saveAllAction, &QAction::triggered, this, [this](){
        if (m_saveAllFilesCallback) m_saveAllFilesCallback();
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
