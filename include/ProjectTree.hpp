#ifndef PROJECTTREE_HPP
#define PROJECTTREE_HPP

// forward decl
class MainWindow;
namespace openide { class AppSettings; }

#include "code/CodeTabPane.hpp"
#include "FileType.hpp"

#include <QString>
#include <QTreeView>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

namespace openide
{
    class ProjectTree : public QTreeView
    {
        Q_OBJECT
    public:
        ProjectTree(MainWindow* parent);
        void loadTreeFromDir(const QString* dirPath);
        ~ProjectTree();
        
    protected:
        void contextMenuEvent(QContextMenuEvent* event) override;
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dragMoveEvent(QDragMoveEvent* event) override;
        void dropEvent(QDropEvent* event) override;
        
    private slots:
        void onItemDoubleClicked(const QModelIndex& index);
        void deleteItem();
        void renameItem();
        void addNewFile();
        void addNewDirectory();
        void collapseAllRecursive();
        void expandAllRecursive();
        
    private:
        void collapseRecursive(const QModelIndex& index);
        void expandRecursive(const QModelIndex& index);
        bool moveDirectory(const QString& source, const QString& target);
        
        MainWindow* m_parent;
        QFileSystemModel* m_fileSystemModel;
        QModelIndex m_contextMenuIndex;
    };
}
#endif // PROJECTTREE_HPP
