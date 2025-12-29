#include "menu/ThemeMenu.hpp"
#include "MainWindow.hpp"
#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include <QActionGroup>

using namespace openide::menu;

ThemeMenu::ThemeMenu(MainWindow* parent, QMenuBar* menuBar)
    : QMenu(parent)
    , m_currentTheme(ThemeMenu::Theme::System)
{
    if (!parent || !menuBar) return;

    QMenu* themeMenu = menuBar->addMenu("&Theme");
    
    // Create action group for exclusive selection
    QActionGroup* themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);
    
    m_systemThemeAction = new QAction("&System Theme", this);
    m_systemThemeAction->setCheckable(true);
    m_systemThemeAction->setChecked(true);
    themeGroup->addAction(m_systemThemeAction);
    
    m_lightThemeAction = new QAction("&Light Theme", this);
    m_lightThemeAction->setCheckable(true);
    themeGroup->addAction(m_lightThemeAction);
    
    m_darkThemeAction = new QAction("&Dark Theme", this);
    m_darkThemeAction->setCheckable(true);
    themeGroup->addAction(m_darkThemeAction);
    
    themeMenu->addAction(m_systemThemeAction);
    themeMenu->addAction(m_lightThemeAction);
    themeMenu->addAction(m_darkThemeAction);
    
    // Connect actions
    connect(m_systemThemeAction, &QAction::triggered, this, &ThemeMenu::onSystemThemeTriggered);
    connect(m_lightThemeAction, &QAction::triggered, this, &ThemeMenu::onLightThemeTriggered);
    connect(m_darkThemeAction, &QAction::triggered, this, &ThemeMenu::onDarkThemeTriggered);
    
    // Don't apply theme in constructor - let MainWindow do it after connections are set up
}

void ThemeMenu::onLightThemeTriggered()
{
    applyTheme(ThemeMenu::Theme::Light);
}

void ThemeMenu::onDarkThemeTriggered()
{
    applyTheme(ThemeMenu::Theme::Dark);
}

void ThemeMenu::onSystemThemeTriggered()
{
    detectSystemTheme();
    applyTheme(ThemeMenu::Theme::System);
}

void ThemeMenu::detectSystemTheme()
{
    // This method is kept for potential future use
    // The actual detection happens in applyTheme when System theme is selected
}

void ThemeMenu::applyTheme(ThemeMenu::Theme theme)
{
    QApplication* app = qApp;
    ThemeMenu::Theme actualTheme = theme;
    
    if (theme == ThemeMenu::Theme::System) {
        // Reset to system default
        app->setStyle(QStyleFactory::create("Fusion"));
        QPalette systemPalette;
        app->setPalette(systemPalette);
        
        // Detect what the system theme actually is
        QPalette palette = QApplication::palette();
        QColor bgColor = palette.color(QPalette::Window);
        int luminance = (299 * bgColor.red() + 587 * bgColor.green() + 114 * bgColor.blue()) / 1000;
        actualTheme = (luminance < 128) ? ThemeMenu::Theme::Dark : ThemeMenu::Theme::Light;
        m_currentTheme = ThemeMenu::Theme::System;
    } else if (theme == ThemeMenu::Theme::Light) {
        // Apply light theme
        app->setStyle(QStyleFactory::create("Fusion"));
        QPalette palette;
        palette.setColor(QPalette::Window, QColor(255, 255, 255));
        palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
        palette.setColor(QPalette::Base, QColor(255, 255, 255));
        palette.setColor(QPalette::AlternateBase, QColor(240, 240, 240));
        palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
        palette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
        palette.setColor(QPalette::Text, QColor(0, 0, 0));
        palette.setColor(QPalette::Button, QColor(240, 240, 240));
        palette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
        palette.setColor(QPalette::BrightText, QColor(255, 0, 0));
        palette.setColor(QPalette::Link, QColor(0, 0, 255));
        palette.setColor(QPalette::Highlight, QColor(0, 120, 215));
        palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
        app->setPalette(palette);
        m_currentTheme = ThemeMenu::Theme::Light;
        actualTheme = ThemeMenu::Theme::Light;
    } else if (theme == ThemeMenu::Theme::Dark) {
        // Apply dark theme
        app->setStyle(QStyleFactory::create("Fusion"));
        QPalette palette;
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, QColor(255, 255, 255));
        palette.setColor(QPalette::Base, QColor(35, 35, 35));
        palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        palette.setColor(QPalette::ToolTipBase, QColor(0, 0, 0));
        palette.setColor(QPalette::ToolTipText, QColor(255, 255, 255));
        palette.setColor(QPalette::Text, QColor(255, 255, 255));
        palette.setColor(QPalette::Button, QColor(53, 53, 53));
        palette.setColor(QPalette::ButtonText, QColor(255, 255, 255));
        palette.setColor(QPalette::BrightText, QColor(255, 0, 0));
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::HighlightedText, QColor(0, 0, 0));
        app->setPalette(palette);
        m_currentTheme = ThemeMenu::Theme::Dark;
        actualTheme = ThemeMenu::Theme::Dark;
    }
    
    // Emit the actual theme (Light or Dark) for CodeEditor updates
    emit themeChanged(actualTheme);
}

