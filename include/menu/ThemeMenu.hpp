#ifndef THEMEMENU_HPP
#define THEMEMENU_HPP

#include <QWidget>
#include <QMenu>
#include <QMenuBar>

// forward decl
class MainWindow;

namespace openide::menu
{
class ThemeMenu : public QMenu
{
    Q_OBJECT
public:
    enum class Theme {
        System,
        Light,
        Dark
    };
    Q_ENUM(Theme)
    ThemeMenu(MainWindow* parent, QMenuBar* menuBar);
    ~ThemeMenu() = default;
    
    void applyTheme(ThemeMenu::Theme theme);
    ThemeMenu::Theme getCurrentTheme() const { return m_currentTheme; }
    
signals:
    void themeChanged(ThemeMenu::Theme theme);
    
private slots:
    void onLightThemeTriggered();
    void onDarkThemeTriggered();
    void onSystemThemeTriggered();
    
private:
    void detectSystemTheme();
    ThemeMenu::Theme m_currentTheme;
    QAction* m_lightThemeAction;
    QAction* m_darkThemeAction;
    QAction* m_systemThemeAction;
};
}

#endif // THEMEMENU_HPP

