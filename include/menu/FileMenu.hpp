#ifndef FILEMENU_HPP
#define FILEMENU_HPP

#include "ProjectTree.hpp"

#include <QWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>

// forward decl
class MainWindow;

namespace openide::menu
{
class FileMenu : public QMenu
{
    Q_OBJECT
public:
    FileMenu(MainWindow* parent, QMenuBar* menuBar, ProjectTree* projectTree);
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
private:
    openide::ProjectTree* m_projectTree;
};
}

#endif // FILEMENU_HPP
