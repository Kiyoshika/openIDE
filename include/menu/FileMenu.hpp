#ifndef FILEMENU_HPP
#define FILEMENU_HPP

#include <QWidget>
#include <QFileDialog>

namespace openide
{
struct FileMenu
{
/**
     * @brief Open a file dialog and write the filePath for selected file
     * @param parent Parent object for the dialog
     * @param Path to the selected file (empty if none)
     */
    static void openFile(QWidget* parent, QString* filePath);

    /**
     * @brief Open a directory dialog and write the dirPath for selected directory
     * @param parent Parent object for the dialog
     * @param Path to the selected directory (empty if none)
     */
    static void openDir(QWidget* parent, QString* dirPath);
};
}

#endif // FILEMENU_HPP
