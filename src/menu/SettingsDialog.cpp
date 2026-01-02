#include "menu/SettingsDialog.hpp"
#include "MainWindow.hpp"
#include "AppSettings.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QFontComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

using namespace openide;
using namespace openide::menu;

SettingsDialog::SettingsDialog(MainWindow* parent, AppSettings* settings)
    : QDialog(parent)
    , m_settings(settings)
    , m_fontComboBox(nullptr)
    , m_fontSizeSpinBox(nullptr)
    , m_tabSpaceSpinBox(nullptr)
    , m_projectTreeFontComboBox(nullptr)
    , m_projectTreeFontSizeSpinBox(nullptr)
    , m_terminalFontComboBox(nullptr)
    , m_terminalFontSizeSpinBox(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
    , m_applyButton(nullptr)
{
    if (!m_settings) {
        return;
    }
    
    setupUI();
    loadSettings();
    
    setWindowTitle("Settings");
    setModal(true);
    resize(450, 500);
}

void SettingsDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    
    // Code Editor section
    QGroupBox* codeEditorGroup = new QGroupBox("Code Editor", this);
    QFormLayout* codeEditorLayout = new QFormLayout(codeEditorGroup);
    
    m_fontComboBox = new QFontComboBox(codeEditorGroup);
    codeEditorLayout->addRow("Font:", m_fontComboBox);
    
    m_fontSizeSpinBox = new QSpinBox(codeEditorGroup);
    m_fontSizeSpinBox->setMinimum(6);
    m_fontSizeSpinBox->setMaximum(72);
    m_fontSizeSpinBox->setValue(12);
    codeEditorLayout->addRow("Font Size:", m_fontSizeSpinBox);
    
    m_tabSpaceSpinBox = new QSpinBox(codeEditorGroup);
    m_tabSpaceSpinBox->setMinimum(1);
    m_tabSpaceSpinBox->setMaximum(16);
    m_tabSpaceSpinBox->setValue(4);
    codeEditorLayout->addRow("Tab Space:", m_tabSpaceSpinBox);
    
    codeEditorGroup->setLayout(codeEditorLayout);
    mainLayout->addWidget(codeEditorGroup);
    
    // Project Tree section
    QGroupBox* projectTreeGroup = new QGroupBox("Project Tree", this);
    QFormLayout* projectTreeLayout = new QFormLayout(projectTreeGroup);
    
    m_projectTreeFontComboBox = new QFontComboBox(projectTreeGroup);
    projectTreeLayout->addRow("Font:", m_projectTreeFontComboBox);
    
    m_projectTreeFontSizeSpinBox = new QSpinBox(projectTreeGroup);
    m_projectTreeFontSizeSpinBox->setMinimum(6);
    m_projectTreeFontSizeSpinBox->setMaximum(72);
    m_projectTreeFontSizeSpinBox->setValue(10);
    projectTreeLayout->addRow("Font Size:", m_projectTreeFontSizeSpinBox);
    
    projectTreeGroup->setLayout(projectTreeLayout);
    mainLayout->addWidget(projectTreeGroup);
    
    // Terminal section
    QGroupBox* terminalGroup = new QGroupBox("Terminal", this);
    QFormLayout* terminalLayout = new QFormLayout(terminalGroup);
    
    m_terminalFontComboBox = new QFontComboBox(terminalGroup);
    terminalLayout->addRow("Font:", m_terminalFontComboBox);
    
    m_terminalFontSizeSpinBox = new QSpinBox(terminalGroup);
    m_terminalFontSizeSpinBox->setMinimum(6);
    m_terminalFontSizeSpinBox->setMaximum(72);
    m_terminalFontSizeSpinBox->setValue(10);
    terminalLayout->addRow("Font Size:", m_terminalFontSizeSpinBox);
    
    terminalGroup->setLayout(terminalLayout);
    mainLayout->addWidget(terminalGroup);
    
    mainLayout->addStretch();
    
    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, this);
    m_okButton = buttonBox->button(QDialogButtonBox::Ok);
    m_cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    m_applyButton = buttonBox->button(QDialogButtonBox::Apply);
    
    mainLayout->addWidget(buttonBox);
    
    // Connect buttons
    connect(m_okButton, &QPushButton::clicked, this, &SettingsDialog::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);
    connect(m_applyButton, &QPushButton::clicked, this, &SettingsDialog::onApplyClicked);
}

void SettingsDialog::loadSettings()
{
    if (!m_settings) return;
    
    // Code Editor settings
    m_fontComboBox->setCurrentFont(QFont(m_settings->fontFamily()));
    m_fontSizeSpinBox->setValue(m_settings->fontSize());
    m_tabSpaceSpinBox->setValue(m_settings->tabSpace());
    
    // Project Tree settings
    m_projectTreeFontComboBox->setCurrentFont(QFont(m_settings->projectTreeFontFamily()));
    m_projectTreeFontSizeSpinBox->setValue(m_settings->projectTreeFontSize());
    
    // Terminal settings
    m_terminalFontComboBox->setCurrentFont(QFont(m_settings->terminalFontFamily()));
    m_terminalFontSizeSpinBox->setValue(m_settings->terminalFontSize());
}

void SettingsDialog::applySettings()
{
    if (!m_settings) return;
    
    // Code Editor settings
    m_settings->setFontFamily(m_fontComboBox->currentFont().family());
    m_settings->setFontSize(m_fontSizeSpinBox->value());
    m_settings->setTabSpace(m_tabSpaceSpinBox->value());
    
    // Project Tree settings
    m_settings->setProjectTreeFontFamily(m_projectTreeFontComboBox->currentFont().family());
    m_settings->setProjectTreeFontSize(m_projectTreeFontSizeSpinBox->value());
    
    // Terminal settings
    m_settings->setTerminalFontFamily(m_terminalFontComboBox->currentFont().family());
    m_settings->setTerminalFontSize(m_terminalFontSizeSpinBox->value());
    
    m_settings->saveToFile();
    emit settingsChanged();
}

void SettingsDialog::onApplyClicked()
{
    applySettings();
}

void SettingsDialog::onOkClicked()
{
    applySettings();
    accept();
}

void SettingsDialog::onCancelClicked()
{
    reject();
}

