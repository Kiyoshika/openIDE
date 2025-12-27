#include "ProjectTree.hpp"
#include "MainWindow.hpp"
#include "FileType.hpp"
#include "CodeTabPane.hpp"

using namespace openide;
using namespace openide::code;

ProjectTree::ProjectTree(MainWindow* parent)
{
    m_parent = parent;
    m_fileSystemModel = new QFileSystemModel(parent->getCentralWidget());
    m_fileSystemModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    m_treeView = new QTreeView(parent->getCentralWidget());
    m_treeView->setModel(m_fileSystemModel);

    // tree styling
    m_treeView->setAnimated(true);
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);
    m_treeView->hideColumn(1); // Size
    m_treeView->hideColumn(2); // Type
    m_treeView->hideColumn(3); // Date Modified
    m_treeView->setHeaderHidden(false);
    m_treeView->setVisible(false);
    m_treeView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // grid placement
    if (parent->getLayout())
        parent->getLayout()->addWidget(m_treeView, 0, 0);
}

QTreeView* ProjectTree::getTreeView() const
{
    return m_treeView;
}

ProjectTree::~ProjectTree()
{
    // do NOT delete m_parent; this is a non-owning pointer
    delete m_fileSystemModel;
    delete m_treeView;
}

void ProjectTree::setComponentVisible(bool isVisible)
{
    this->m_treeView->setVisible(isVisible);
}

void ProjectTree::loadTreeFromDir(const QString* dirPath)
{
    if (!dirPath) return;

    // set root path pointed to by tree
    m_fileSystemModel->setRootPath(*dirPath);
    QModelIndex rootIndex = m_fileSystemModel->index(*dirPath);
    m_treeView->setRootIndex(rootIndex);

    this->m_parent->setComponentsVisible(true);
}

void ProjectTree::onClick(CodeTabPane& codeTabPane, const QModelIndex& index)
{
    QFileSystemModel* model = qobject_cast<QFileSystemModel*>(this->m_treeView->model());
    if (!model) return;

    if (model->isDir(index))
    {
        return;
    } else
    {
        const QString& path = model->filePath(index);
        if (path.length() == 0) return;
        enum FileType fileType = FileType::UNKNOWN;

        // has file extension
        int dotIndex = -1;
        if ((dotIndex = path.lastIndexOf(".")) != -1 && dotIndex != path.length() - 1)
        {
            QString fileExtension = path.right(path.length() - (dotIndex + 1)).toLower();
            fileType = FileTypeUtil::fromExtension(fileExtension);
        }

        // grab file name
        int slashIndex = -1;
        QString fileName = "";
        if (((slashIndex = path.lastIndexOf("/")) != -1) || ((slashIndex = path.lastIndexOf("\\")) != -1))
            fileName = path.right(path.length() - (slashIndex + 1));
        else
            fileName = path;

        CodeEditor* newEditor = new CodeEditor(m_parent);
        newEditor->loadFile(path, fileType);
        codeTabPane.addTab(newEditor, fileName);
    }
}
