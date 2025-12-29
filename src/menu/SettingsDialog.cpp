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

using namespace openide;
using namespace openide::menu;

SettingsDialog::SettingsDialog(MainWindow* parent, AppSettings* settings)
    : QDialog(parent)
    , m_settings(settings)
    , m_fontComboBox(nullptr)
    , m_fontSizeSpinBox(nullptr)
    , m_tabSpaceSpinBox(nullptr)
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
    resize(400, 200);
}

void SettingsDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QFormLayout* formLayout = new QFormLayout();
    
    // Font selection
    m_fontComboBox = new QFontComboBox(this);
    formLayout->addRow("Font:", m_fontComboBox);
    
    // Font size
    m_fontSizeSpinBox = new QSpinBox(this);
    m_fontSizeSpinBox->setMinimum(6);
    m_fontSizeSpinBox->setMaximum(72);
    m_fontSizeSpinBox->setValue(12);
    formLayout->addRow("Font Size:", m_fontSizeSpinBox);
    
    // Tab space
    m_tabSpaceSpinBox = new QSpinBox(this);
    m_tabSpaceSpinBox->setMinimum(1);
    m_tabSpaceSpinBox->setMaximum(16);
    m_tabSpaceSpinBox->setValue(4);
    formLayout->addRow("Tab Space:", m_tabSpaceSpinBox);
    
    mainLayout->addLayout(formLayout);
    
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
    
    m_fontComboBox->setCurrentFont(QFont(m_settings->fontFamily()));
    m_fontSizeSpinBox->setValue(m_settings->fontSize());
    m_tabSpaceSpinBox->setValue(m_settings->tabSpace());
}

void SettingsDialog::applySettings()
{
    if (!m_settings) return;
    
    m_settings->setFontFamily(m_fontComboBox->currentFont().family());
    m_settings->setFontSize(m_fontSizeSpinBox->value());
    m_settings->setTabSpace(m_tabSpaceSpinBox->value());
    
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

