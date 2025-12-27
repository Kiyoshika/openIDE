#ifndef PROJECTTREE_HPP
#define PROJECTTREE_HPP

#include <QString>
#include <QTreeView>
#include <QFileSystemModel>
#include <QModelIndex>

namespace openide::projecttree
{
void loadTreeFromDir(QWidget* parent, const QString* dirPath, QTreeView* projectTree);
}
#endif // PROJECTTREE_HPP
