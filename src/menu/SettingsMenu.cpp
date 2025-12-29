#include "menu/SettingsMenu.hpp"
#include "menu/SettingsDialog.hpp"
#include "MainWindow.hpp"
#include "code/CodeTabPane.hpp"

using namespace openide;
using namespace openide::menu;

SettingsMenu::SettingsMenu(MainWindow* parent, QMenuBar* menuBar, AppSettings* settings)
    : QMenu(parent)
    , m_mainWindow(parent)
    , m_settings(settings)
{
    if (!parent || !menuBar) return;
    
    QMenu* settingsMenu = menuBar->addMenu("&Settings");
    QAction* settingsAction = new QAction("&Preferences...", this);
    settingsMenu->addAction(settingsAction);
    
    connect(settingsAction, &QAction::triggered, this, &SettingsMenu::onSettingsTriggered);
}

void SettingsMenu::onSettingsTriggered()
{
    if (!m_settings || !m_mainWindow) return;
    
    SettingsDialog dialog(m_mainWindow, m_settings);
    // Connect dialog's signal directly to update editors and also forward to menu's signal
    connect(&dialog, &SettingsDialog::settingsChanged, this, [this](){
        // Update all editors immediately
        m_mainWindow->getCodeTabPane()->updateAllEditorsSettings(m_settings);
        // Also emit the signal for any other listeners
        emit settingsChanged();
    });
    dialog.exec();
}

