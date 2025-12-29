#ifndef NEWPROJECTDIALOG_HPP
#define NEWPROJECTDIALOG_HPP

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
class NewProjectDialog : public QDialog
{
    Q_OBJECT
public:
    NewProjectDialog(MainWindow* parent);
    ~NewProjectDialog() = default;
    
    QString getProjectPath() const { return m_projectPath; }
    QString getProjectName() const { return m_projectName; }
    
private slots:
    void onBrowseClicked();
    void onProjectNameChanged(const QString& text);
    void onOkClicked();
    void onCancelClicked();
    
private:
    void setupUI();
    QString convertProjectName(const QString& input);
    bool validateProjectDirectory(const QString& basePath, const QString& projectName);
    
    QString m_projectPath;
    QString m_projectName;
    QLineEdit* m_directoryLineEdit;
    QPushButton* m_browseButton;
    QLineEdit* m_projectNameLineEdit;
    QLabel* m_previewLabel;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
};
}

#endif // NEWPROJECTDIALOG_HPP


