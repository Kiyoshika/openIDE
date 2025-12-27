#include "ProjectTree.hpp"

using namespace openide;

void projecttree::loadTreeFromDir(QWidget* parent, const QString* dirPath, QTreeView* projectTree)
{
    if (!parent || !dirPath || !projectTree) return;

    QFileSystemModel* model = new QFileSystemModel(parent);
    model->setRootPath(*dirPath);
    model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);

    projectTree->setModel(model);

    // set root path pointed to by tree
    QModelIndex rootIndex = model->index(*dirPath);
    projectTree->setRootIndex(rootIndex);

    // tree stylijng
    projectTree->setAnimated(true);
    projectTree->setSortingEnabled(true);
    projectTree->sortByColumn(0, Qt::AscendingOrder);
    projectTree->hideColumn(1); // Size
    projectTree->hideColumn(2); // Type
    projectTree->hideColumn(3); // Date Modified

    projectTree->setHeaderHidden(false);
}
