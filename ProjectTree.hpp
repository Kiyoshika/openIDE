#ifndef PROJECTTREE_HPP
#define PROJECTTREE_HPP

// forward decl
class MainWindow;

#include "CodeEditor.hpp"

#include <QString>
#include <QTreeView>
#include <QFileSystemModel>
#include <QModelIndex>

namespace openide
{
    class ProjectTree
    {
    public:
        ProjectTree(MainWindow* parent);
        QTreeView* getTreeView() const;
        void loadTreeFromDir(const QString* dirPath);
        void setComponentVisible(bool isVisible);
        void onClick(CodeEditor& codeEditor, const QModelIndex& index);
        ~ProjectTree();
    private:
        MainWindow* m_parent;
        QFileSystemModel* m_fileSystemModel;
        QTreeView* m_treeView;
    };
}
#endif // PROJECTTREE_HPP
