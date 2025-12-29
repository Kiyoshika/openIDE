#include "ProjectTree.hpp"
#include "MainWindow.hpp"
#include "FileType.hpp"
#include "code/CodeTabPane.hpp"
#include "code/CodeEditor.hpp"
#include "AppSettings.hpp"

using namespace openide;
using namespace openide::code;

ProjectTree::ProjectTree(MainWindow* parent)
    : QTreeView(parent)
    , m_parent(parent)
    , m_fileSystemModel(new QFileSystemModel(this))
{
    m_fileSystemModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    setModel(m_fileSystemModel);

    // tree styling
    setAnimated(true);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    hideColumn(1); // Size
    hideColumn(2); // Type
    hideColumn(3); // Date Modified
    setHeaderHidden(false);
    setVisible(false);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // grid placement
    if (parent && parent->getLayout())
        parent->getLayout()->addWidget(this, 0, 0);
    
    // Connect double-click to our handler
    connect(this, &QTreeView::doubleClicked, this, &ProjectTree::onItemDoubleClicked);
}

ProjectTree::~ProjectTree()
{
    // QTreeView destructor will handle cleanup
    // m_fileSystemModel will be deleted by Qt's parent-child ownership (this is its parent)
}

void ProjectTree::loadTreeFromDir(const QString* dirPath)
{
    if (!dirPath) return;

    // set root path pointed to by tree
    m_fileSystemModel->setRootPath(*dirPath);
    QModelIndex rootIndex = m_fileSystemModel->index(*dirPath);
    setRootIndex(rootIndex);

    if (m_parent)
        m_parent->setComponentsVisible(true);
}

void ProjectTree::onItemDoubleClicked(const QModelIndex& index)
{
    if (!m_parent) return;
    
    QFileSystemModel* model = qobject_cast<QFileSystemModel*>(this->model());
    if (!model) return;

    // Ignore directories
    if (model->isDir(index)) return;

    const QString& path = model->filePath(index);
    if (path.isEmpty()) return;
    
    FileType fileType = FileType::UNKNOWN;

    // has file extension
    int dotIndex = path.lastIndexOf(".");
    if (dotIndex != -1 && dotIndex != path.length() - 1)
    {
        QString fileExtension = path.right(path.length() - (dotIndex + 1)).toLower();
        fileType = FileTypeUtil::fromExtension(fileExtension);
    }

    // grab file name
    int slashIndex = -1;
    QString fileName;
    if (((slashIndex = path.lastIndexOf("/")) != -1) || ((slashIndex = path.lastIndexOf("\\")) != -1))
        fileName = path.right(path.length() - (slashIndex + 1));
    else
        fileName = path;

    // Open file in CodeTabPane
    code::CodeTabPane* codeTabPane = m_parent->getCodeTabPane();
    if (codeTabPane && !codeTabPane->fileIsOpen(path)) {
        code::CodeEditor* newEditor = new code::CodeEditor(m_parent, m_parent->getAppSettings());
        newEditor->loadFile(path, fileType);
        codeTabPane->addTab(newEditor, fileName);
    }
}
