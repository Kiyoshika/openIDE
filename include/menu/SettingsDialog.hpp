#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include "AppSettings.hpp"

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFontComboBox>
#include <QFormLayout>
#include <QGroupBox>

// forward decl
class MainWindow;

namespace openide::menu
{
class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    SettingsDialog(MainWindow* parent, openide::AppSettings* settings);
    ~SettingsDialog() = default;
    
private slots:
    void onApplyClicked();
    void onCancelClicked();
    void onOkClicked();
    
signals:
    void settingsChanged();
    
private:
    void setupUI();
    void loadSettings();
    void applySettings();
    
    openide::AppSettings* m_settings;
    // Code Editor section
    QFontComboBox* m_fontComboBox;
    QSpinBox* m_fontSizeSpinBox;
    QSpinBox* m_tabSpaceSpinBox;
    // Project Tree section
    QFontComboBox* m_projectTreeFontComboBox;
    QSpinBox* m_projectTreeFontSizeSpinBox;
    // Terminal section
    QFontComboBox* m_terminalFontComboBox;
    QSpinBox* m_terminalFontSizeSpinBox;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QPushButton* m_applyButton;
};
}

#endif // SETTINGSDIALOG_HPP

