#ifndef THEMEMENU_HPP
#define THEMEMENU_HPP

#include <QWidget>
#include <QMenu>
#include <QMenuBar>

// forward decl
class MainWindow;

namespace openide::menu
{
enum class Theme {
    System,
    Light,
    Dark
};

class ThemeMenu : public QMenu
{
    Q_OBJECT
public:
    ThemeMenu(MainWindow* parent, QMenuBar* menuBar);
    ~ThemeMenu() = default;
    
    void applyTheme(Theme theme);
    Theme getCurrentTheme() const { return m_currentTheme; }
    
signals:
    void themeChanged(Theme theme);
    
private slots:
    void onLightThemeTriggered();
    void onDarkThemeTriggered();
    void onSystemThemeTriggered();
    
private:
    void detectSystemTheme();
    Theme m_currentTheme;
    QAction* m_lightThemeAction;
    QAction* m_darkThemeAction;
    QAction* m_systemThemeAction;
};
}

#endif // THEMEMENU_HPP

