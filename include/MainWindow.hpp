#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "menu/FileMenu.hpp"
#include "menu/ThemeMenu.hpp"
#include "menu/SettingsMenu.hpp"
#include "AppSettings.hpp"
#include "ProjectTree.hpp"
#include "code/CodeEditor.hpp"
#include "code/CodeTabPane.hpp"

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QObject>
#include <QTreeView>
#include <QLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QWidget* getCentralWidget() const;
    QGridLayout* getLayout() const;
    void setComponentsVisible(bool isVisible);
    openide::AppSettings* getAppSettings() { return &m_appSettings; }
    openide::code::CodeTabPane* getCodeTabPane() { return &m_codeTabPane; }
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QWidget* m_centralWidget;
    QGridLayout* m_layout;
    openide::ProjectTree m_projectTree;
    openide::code::CodeTabPane m_codeTabPane;
    openide::menu::FileMenu m_fileMenu;
    openide::menu::ThemeMenu m_themeMenu;
    openide::menu::SettingsMenu m_settingsMenu;
    openide::AppSettings m_appSettings;
};

#endif // MAINWINDOW_HPP
