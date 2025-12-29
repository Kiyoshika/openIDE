#include "menu/FileMenu.hpp"
#include "menu/NewProjectDialog.hpp"
#include "menu/NewFileDialog.hpp"
#include "ProjectTree.hpp"
#include "MainWindow.hpp"
#include "code/CodeEditor.hpp"
#include "code/CodeTabPane.hpp"
#include "FileType.hpp"
#include <QDir>
#include <QFileInfo>

using namespace openide;
using namespace openide::menu;

FileMenu::FileMenu(
    MainWindow* parent,
    QMenuBar* menuBar,
    ProjectTree* projectTree,
    openide::code::CodeTabPane* codeTabPane)
    : QMenu(parent)
    , m_mainWindow(parent)
    , m_projectTree{projectTree}
    , m_codeTabPane{codeTabPane}
    , m_saveAction(nullptr)
    , m_saveAllAction(nullptr)
    , m_newFileAction(nullptr)
{
    if (!parent || !menuBar) return;

    QMenu* fileMenu = menuBar->addMenu("&File");
    
    // Project section
    QAction* newAction = new QAction("&New Project", this);
    QAction* openAction = new QAction("&Open Project", this);
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    
    // Separator
    fileMenu->addSeparator();
    
    // Files section
    m_saveAction = new QAction("&Save File", this);
    m_saveAllAction = new QAction("Save &All Files", this);
    m_newFileAction = new QAction("&New File", this);
    fileMenu->addAction(m_saveAction);
    fileMenu->addAction(m_saveAllAction);
    fileMenu->addAction(m_newFileAction);

    // Initially disable file actions (no project open)
    setProjectActionsEnabled(false);

    // Connect actions
    connect(newAction, &QAction::triggered, this, &FileMenu::onNewProjectTriggered);
    connect(openAction, &QAction::triggered, this, &FileMenu::onOpenProjectTriggered);
    connect(m_saveAction, &QAction::triggered, this, [this](){
        if (m_codeTabPane) {
            m_codeTabPane->saveActiveFile();
        }
    });
    connect(m_saveAllAction, &QAction::triggered, this, [this](){
        if (m_codeTabPane) {
            m_codeTabPane->saveAllActiveFiles();
        }
    });
    connect(m_newFileAction, &QAction::triggered, this, &FileMenu::onNewFileTriggered);
}

void FileMenu::setProjectActionsEnabled(bool enabled)
{
    if (m_saveAction) m_saveAction->setEnabled(enabled);
    if (m_saveAllAction) m_saveAllAction->setEnabled(enabled);
    if (m_newFileAction) m_newFileAction->setEnabled(enabled);
}

void FileMenu::onNewProjectTriggered()
{
    if (!m_mainWindow) return;
    
    NewProjectDialog dialog(m_mainWindow);
    if (dialog.exec() == QDialog::Accepted) {
        QString projectPath = dialog.getProjectPath();
        QString projectName = dialog.getProjectName();
        
        if (!projectPath.isEmpty() && !projectName.isEmpty()) {
            // Load the project tree
            m_projectTree->loadTreeFromDir(&projectPath);
            // Emit signal for title update and enable file actions
            emit projectOpened(projectPath, projectName);
            setProjectActionsEnabled(true);
        }
    }
}

void FileMenu::onOpenProjectTriggered()
{
    QString filePath;
    FileMenu::openDir(this, &filePath);
    if (!filePath.isEmpty()) {
        m_projectTree->loadTreeFromDir(&filePath);
        
        // Extract project name from path (last directory name)
        QDir dir(filePath);
        QString projectName = dir.dirName();
        emit projectOpened(filePath, projectName);
        setProjectActionsEnabled(true);
    }
}

void FileMenu::openFile(QWidget* parent, QString* filePath)
{
    if (!parent || !filePath) return;
    QString selectedFilePath = QFileDialog::getOpenFileName(parent, "Open File", QDir::homePath(), "All Files (*)");
    *filePath = selectedFilePath;
}

void FileMenu::onNewFileTriggered()
{
    if (!m_mainWindow) return;
    
    // Get default directory (project root if available, otherwise home)
    QString defaultDir = m_mainWindow->getCurrentProjectRoot();
    // If no project root is set, we shouldn't be here (action should be disabled)
    // But as a fallback, use home directory
    if (defaultDir.isEmpty()) {
        defaultDir = QDir::homePath();
    }
    
    // Ensure the directory path is normalized
    QDir dir(defaultDir);
    defaultDir = dir.absolutePath();
    
    NewFileDialog dialog(m_mainWindow, defaultDir);
    if (dialog.exec() == QDialog::Accepted) {
        QString filePath = dialog.getFilePath();
        
        if (!filePath.isEmpty()) {
            // Determine file type from extension
            QFileInfo fileInfo(filePath);
            QString extension = fileInfo.suffix().toLower();
            enum FileType fileType = FileTypeUtil::fromExtension(extension);
            
            // Get file name for tab
            QString fileName = fileInfo.fileName();
            
            // Create CodeEditor and load the file
            openide::code::CodeEditor* newEditor = new openide::code::CodeEditor(m_mainWindow, m_mainWindow->getAppSettings());
            newEditor->loadFile(filePath, fileType);
            
            // Add to CodeTabPane
            m_mainWindow->getCodeTabPane()->addTab(newEditor, fileName);
            
            // Make components visible if not already
            m_mainWindow->setComponentsVisible(true);
        }
    }
}

void FileMenu::openDir(QWidget* parent, QString* dirPath)
{
    if (!parent || !dirPath) return;
    QString selectedDirPath = QFileDialog::getExistingDirectory(parent, "Open Direcotry", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    *dirPath = selectedDirPath;
}
