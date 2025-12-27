#include "FileMenu.hpp"
#include "ProjectTree.hpp"

using namespace openide;

void filemenu::openFile(QWidget* parent, QString* filePath)
{
    if (!parent || !filePath) return;
    QString selectedFilePath = QFileDialog::getOpenFileName(parent, "Open File", QDir::homePath(), "All Files (*)");
    *filePath = selectedFilePath;
}

void filemenu::openDir(QWidget* parent, QString* dirPath)
{
    if (!parent || !dirPath) return;
    QString selectedDirPath = QFileDialog::getExistingDirectory(parent, "Open Direcotry", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    *dirPath = selectedDirPath;
}
