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
    
    // Enable drag and drop
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    
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

void ProjectTree::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    
    // If clicking on empty space, use project root
    if (!index.isValid()) {
        // Get the root index (project root)
        index = rootIndex();
        if (!index.isValid()) {
            return;
        }
    }
    
    m_contextMenuIndex = index;
    QFileInfo fileInfo = m_fileSystemModel->fileInfo(index);
    bool isDir = fileInfo.isDir();
    
    QMenu menu(this);
    
    // Common actions for both files and directories
    QAction* deleteAction = menu.addAction("Delete");
    QAction* renameAction = menu.addAction("Rename");
    menu.addSeparator();
    QAction* newFileAction = menu.addAction("New File");
    QAction* newDirAction = menu.addAction("New Directory");
    
    // Directory-specific actions
    QAction* collapseAction = nullptr;
    QAction* expandAction = nullptr;
    if (isDir) {
        menu.addSeparator();
        collapseAction = menu.addAction("Collapse All");
        expandAction = menu.addAction("Expand All");
    }
    
    // Connect actions to slots
    connect(deleteAction, &QAction::triggered, this, &ProjectTree::deleteItem);
    connect(renameAction, &QAction::triggered, this, &ProjectTree::renameItem);
    connect(newFileAction, &QAction::triggered, this, &ProjectTree::addNewFile);
    connect(newDirAction, &QAction::triggered, this, &ProjectTree::addNewDirectory);
    
    if (isDir) {
        connect(collapseAction, &QAction::triggered, this, &ProjectTree::collapseAllRecursive);
        connect(expandAction, &QAction::triggered, this, &ProjectTree::expandAllRecursive);
    }
    
    menu.exec(event->globalPos());
}

void ProjectTree::deleteItem()
{
    if (!m_contextMenuIndex.isValid()) return;
    
    QFileInfo fileInfo = m_fileSystemModel->fileInfo(m_contextMenuIndex);
    QString path = fileInfo.absoluteFilePath();
    QString name = fileInfo.fileName();
    
    // Confirmation dialog
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Delete",
        QString("Are you sure you want to delete '%1'?").arg(name),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply != QMessageBox::Yes) return;
    
    bool success = false;
    if (fileInfo.isDir()) {
        QDir dir(path);
        success = dir.removeRecursively();
    } else {
        success = QFile::remove(path);
    }
    
    if (!success) {
        QMessageBox::critical(
            this,
            "Delete Failed",
            QString("Failed to delete '%1'. Check permissions.").arg(name)
        );
    }
}

void ProjectTree::renameItem()
{
    if (!m_contextMenuIndex.isValid()) return;
    
    QFileInfo fileInfo = m_fileSystemModel->fileInfo(m_contextMenuIndex);
    QString oldPath = fileInfo.absoluteFilePath();
    QString oldName = fileInfo.fileName();
    QString parentPath = fileInfo.absolutePath();
    
    bool ok;
    QString newName = QInputDialog::getText(
        this,
        "Rename",
        "Enter new name:",
        QLineEdit::Normal,
        oldName,
        &ok
    );
    
    if (!ok || newName.isEmpty() || newName == oldName) return;
    
    // Validation: no path separators
    if (newName.contains('/') || newName.contains('\\')) {
        QMessageBox::warning(
            this,
            "Invalid Name",
            "Name cannot contain path separators (/ or \\)."
        );
        return;
    }
    
    QString newPath = parentPath + "/" + newName;
    
    // Check for conflicts
    if (QFile::exists(newPath)) {
        QMessageBox::warning(
            this,
            "Name Conflict",
            QString("A file or directory named '%1' already exists.").arg(newName)
        );
        return;
    }
    
    bool success = QFile::rename(oldPath, newPath);
    
    if (!success) {
        QMessageBox::critical(
            this,
            "Rename Failed",
            QString("Failed to rename '%1'. Check permissions.").arg(oldName)
        );
    }
}

void ProjectTree::addNewFile()
{
    if (!m_contextMenuIndex.isValid()) return;
    
    QFileInfo fileInfo = m_fileSystemModel->fileInfo(m_contextMenuIndex);
    QString parentPath;
    
    // If clicking on a directory, create file in it; if file, create sibling
    if (fileInfo.isDir()) {
        parentPath = fileInfo.absoluteFilePath();
    } else {
        parentPath = fileInfo.absolutePath();
    }
    
    bool ok;
    QString fileName = QInputDialog::getText(
        this,
        "New File",
        "Enter file name:",
        QLineEdit::Normal,
        "",
        &ok
    );
    
    if (!ok || fileName.isEmpty()) return;
    
    // Validation: no path separators
    if (fileName.contains('/') || fileName.contains('\\')) {
        QMessageBox::warning(
            this,
            "Invalid Name",
            "File name cannot contain path separators (/ or \\)."
        );
        return;
    }
    
    QString newFilePath = parentPath + "/" + fileName;
    
    // Check for conflicts
    if (QFile::exists(newFilePath)) {
        QMessageBox::warning(
            this,
            "Name Conflict",
            QString("A file or directory named '%1' already exists.").arg(fileName)
        );
        return;
    }
    
    QFile file(newFilePath);
    bool success = file.open(QIODevice::WriteOnly);
    if (success) {
        file.close();
    } else {
        QMessageBox::critical(
            this,
            "Create File Failed",
            QString("Failed to create file '%1'. Check permissions.").arg(fileName)
        );
    }
}

void ProjectTree::addNewDirectory()
{
    if (!m_contextMenuIndex.isValid()) return;
    
    QFileInfo fileInfo = m_fileSystemModel->fileInfo(m_contextMenuIndex);
    QString parentPath;
    
    // If clicking on a directory, create subdirectory; if file, create sibling
    if (fileInfo.isDir()) {
        parentPath = fileInfo.absoluteFilePath();
    } else {
        parentPath = fileInfo.absolutePath();
    }
    
    bool ok;
    QString dirName = QInputDialog::getText(
        this,
        "New Directory",
        "Enter directory name:",
        QLineEdit::Normal,
        "",
        &ok
    );
    
    if (!ok || dirName.isEmpty()) return;
    
    // Validation: no path separators
    if (dirName.contains('/') || dirName.contains('\\')) {
        QMessageBox::warning(
            this,
            "Invalid Name",
            "Directory name cannot contain path separators (/ or \\)."
        );
        return;
    }
    
    QString newDirPath = parentPath + "/" + dirName;
    
    // Check for conflicts
    if (QFile::exists(newDirPath)) {
        QMessageBox::warning(
            this,
            "Name Conflict",
            QString("A file or directory named '%1' already exists.").arg(dirName)
        );
        return;
    }
    
    QDir dir(parentPath);
    bool success = dir.mkdir(dirName);
    
    if (!success) {
        QMessageBox::critical(
            this,
            "Create Directory Failed",
            QString("Failed to create directory '%1'. Check permissions.").arg(dirName)
        );
    }
}

void ProjectTree::collapseAllRecursive()
{
    if (!m_contextMenuIndex.isValid()) return;
    collapseRecursive(m_contextMenuIndex);
}

void ProjectTree::expandAllRecursive()
{
    if (!m_contextMenuIndex.isValid()) return;
    expandRecursive(m_contextMenuIndex);
}

void ProjectTree::collapseRecursive(const QModelIndex& index)
{
    if (!index.isValid()) return;
    
    // Collapse all children first
    int rowCount = m_fileSystemModel->rowCount(index);
    for (int i = 0; i < rowCount; ++i) {
        QModelIndex child = m_fileSystemModel->index(i, 0, index);
        if (m_fileSystemModel->isDir(child)) {
            collapseRecursive(child);
        }
    }
    
    // Then collapse this index
    collapse(index);
}

void ProjectTree::expandRecursive(const QModelIndex& index)
{
    if (!index.isValid()) return;
    
    // Don't expand if not a directory
    if (!m_fileSystemModel->isDir(index)) return;
    
    // Expand this index first
    expand(index);
    
    // Get the directory path
    QString dirPath = m_fileSystemModel->filePath(index);
    QDir dir(dirPath);
    
    // Get all subdirectories using QDir (not relying on model)
    QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    
    // For each subdirectory, find its model index and expand it
    for (const QFileInfo& entry : entries) {
        QString childPath = entry.absoluteFilePath();
        QModelIndex childIndex = m_fileSystemModel->index(childPath);
        
        if (childIndex.isValid()) {
            // Recursively expand this child
            expandRecursive(childIndex);
        }
    }
}

bool ProjectTree::moveDirectory(const QString& source, const QString& target)
{
    QDir sourceDir(source);
    QDir targetDir(target);
    
    // Create target directory
    if (!targetDir.exists()) {
        if (!QDir().mkpath(target)) {
            return false;
        }
    }
    
    // Get all entries in source
    QFileInfoList entries = sourceDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    
    for (const QFileInfo& entry : entries) {
        QString sourcePath = entry.absoluteFilePath();
        QString targetPath = target + "/" + entry.fileName();
        
        if (entry.isDir()) {
            // Recursively move directory
            if (!moveDirectory(sourcePath, targetPath)) {
                return false;
            }
        } else {
            // Move file
            if (!QFile::rename(sourcePath, targetPath)) {
                return false;
            }
        }
    }
    
    // Remove source directory after moving all contents
    return sourceDir.removeRecursively();
}

void ProjectTree::dragEnterEvent(QDragEnterEvent* event)
{
    // Accept internal drags
    if (event->source() == this) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void ProjectTree::dragMoveEvent(QDragMoveEvent* event)
{
    QModelIndex targetIndex = indexAt(event->position().toPoint());
    
    // If dropping on empty space, allow it (will use project root)
    if (!targetIndex.isValid()) {
        event->acceptProposedAction();
        return;
    }
    
    // Only allow dropping on directories
    if (!m_fileSystemModel->isDir(targetIndex)) {
        event->ignore();
        return;
    }
    
    event->acceptProposedAction();
}

void ProjectTree::dropEvent(QDropEvent* event)
{
    QModelIndex targetIndex = indexAt(event->position().toPoint());
    QModelIndexList selectedIndexes = selectionModel()->selectedIndexes();
    
    if (selectedIndexes.isEmpty()) {
        event->ignore();
        return;
    }
    
    // If dropping on empty space, use project root
    if (!targetIndex.isValid()) {
        targetIndex = rootIndex();
        if (!targetIndex.isValid()) {
            event->ignore();
            return;
        }
    }
    
    QModelIndex sourceIndex = selectedIndexes.first();
    QFileInfo sourceInfo = m_fileSystemModel->fileInfo(sourceIndex);
    QFileInfo targetInfo = m_fileSystemModel->fileInfo(targetIndex);
    
    QString sourcePath = sourceInfo.absoluteFilePath();
    QString targetPath = targetInfo.absoluteFilePath();
    
    // Prevent moving into self or child
    if (targetPath.startsWith(sourcePath)) {
        QMessageBox::warning(
            this,
            "Invalid Move",
            "Cannot move a directory into itself or its subdirectory."
        );
        event->ignore();
        return;
    }
    
    QString newPath = targetPath + "/" + sourceInfo.fileName();
    
    // Check if target already exists
    if (QFile::exists(newPath)) {
        QMessageBox::warning(
            this,
            "Name Conflict",
            QString("'%1' already exists in the target directory.").arg(sourceInfo.fileName())
        );
        event->ignore();
        return;
    }
    
    // Confirmation dialog - show "project root" if dropping on root
    QString targetName;
    if (targetIndex == rootIndex()) {
        targetName = "project root";
    } else {
        targetName = targetInfo.fileName();
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Move",
        QString("Move '%1' to '%2'?").arg(sourceInfo.fileName(), targetName),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply != QMessageBox::Yes) {
        event->ignore();
        return;
    }
    
    bool success = false;
    if (sourceInfo.isDir()) {
        success = moveDirectory(sourcePath, newPath);
    } else {
        success = QFile::rename(sourcePath, newPath);
    }
    
    if (success) {
        event->acceptProposedAction();
    } else {
        QMessageBox::critical(
            this,
            "Move Failed",
            QString("Failed to move '%1'. Check permissions.").arg(sourceInfo.fileName())
        );
        event->ignore();
    }
}
