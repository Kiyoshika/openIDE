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
    QFontComboBox* m_fontComboBox;
    QSpinBox* m_fontSizeSpinBox;
    QSpinBox* m_tabSpaceSpinBox;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QPushButton* m_applyButton;
};
}

#endif // SETTINGSDIALOG_HPP

