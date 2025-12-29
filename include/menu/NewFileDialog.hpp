#ifndef NEWFILEDIALOG_HPP
#define NEWFILEDIALOG_HPP

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

// forward decl
class MainWindow;

namespace openide::menu
{
class NewFileDialog : public QDialog
{
    Q_OBJECT
public:
    NewFileDialog(MainWindow* parent, const QString& defaultDirectory = "");
    ~NewFileDialog() = default;
    
    QString getFilePath() const { return m_filePath; }
    
private slots:
    void onBrowseClicked();
    void onFileNameChanged(const QString& text);
    void onOkClicked();
    void onCancelClicked();
    
private:
    void setupUI();
    bool isValidFileName(const QString& fileName);
    bool createFile(const QString& filePath);
    
    QString m_filePath;
    QString m_defaultDirectory;
    QLineEdit* m_directoryLineEdit;
    QPushButton* m_browseButton;
    QLineEdit* m_fileNameLineEdit;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
};
}

#endif // NEWFILEDIALOG_HPP

