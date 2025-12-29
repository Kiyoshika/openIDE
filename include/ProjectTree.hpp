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

namespace openide
{
    class ProjectTree : public QTreeView
    {
        Q_OBJECT
    public:
        ProjectTree(MainWindow* parent);
        void loadTreeFromDir(const QString* dirPath);
        ~ProjectTree();
        
    private slots:
        void onItemDoubleClicked(const QModelIndex& index);
        
    private:
        MainWindow* m_parent;
        QFileSystemModel* m_fileSystemModel;
    };
}
#endif // PROJECTTREE_HPP
