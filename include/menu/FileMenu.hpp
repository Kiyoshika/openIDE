#ifndef FILEMENU_HPP
#define FILEMENU_HPP

#include "ProjectTree.hpp"

#include <QWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>

#include <functional>

// forward decl
class MainWindow;

namespace openide::menu
{
class FileMenu : public QMenu
{
    Q_OBJECT
public:
    FileMenu(
        MainWindow* parent,
        QMenuBar* menuBar,
        ProjectTree* projectTree,
        openide::code::CodeTabPane* codeTabPane);
    ~FileMenu() = default;

    /**
     * @brief Open a file dialog and write the filePath for selected file
     * @param parent Parent object for the dialog
     * @param Path to the selected file (empty if none)
     */
    void openFile(QWidget* parent, QString* filePath);

    /**
     * @brief Open a directory dialog and write the dirPath for selected directory
     * @param parent Parent object for the dialog
     * @param Path to the selected directory (empty if none)
     */
    void openDir(QWidget* parent, QString* dirPath);
    
signals:
    void projectOpened(const QString& projectPath, const QString& projectName);
    
public:
    void setProjectActionsEnabled(bool enabled);
    
private slots:
    void onNewProjectTriggered();
    void onOpenProjectTriggered();
    void onNewFileTriggered();
    
private:
    MainWindow* m_mainWindow;
    openide::ProjectTree* m_projectTree;
    openide::code::CodeTabPane* m_codeTabPane;
    QAction* m_saveAction;
    QAction* m_saveAllAction;
    QAction* m_newFileAction;
};
}

#endif // FILEMENU_HPP
