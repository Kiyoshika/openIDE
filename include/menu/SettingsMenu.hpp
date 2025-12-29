#ifndef SETTINGSMENU_HPP
#define SETTINGSMENU_HPP

#include "AppSettings.hpp"

#include <QWidget>
#include <QMenu>
#include <QMenuBar>

// forward decl
class MainWindow;

namespace openide::menu
{
class SettingsMenu : public QMenu
{
    Q_OBJECT
public:
    SettingsMenu(MainWindow* parent, QMenuBar* menuBar, openide::AppSettings* settings);
    ~SettingsMenu() = default;
    
private slots:
    void onSettingsTriggered();
    
signals:
    void settingsChanged();
    
private:
    MainWindow* m_mainWindow;
    openide::AppSettings* m_settings;
};
}

#endif // SETTINGSMENU_HPP

