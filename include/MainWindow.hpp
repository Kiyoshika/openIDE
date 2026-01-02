#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "menu/FileMenu.hpp"
#include "menu/EditMenu.hpp"
#include "menu/ThemeMenu.hpp"
#include "menu/SettingsMenu.hpp"
#include "menu/TerminalMenu.hpp"
#include "AppSettings.hpp"
#include "ProjectTree.hpp"
#include "code/CodeEditor.hpp"
#include "code/CodeTabPane.hpp"
#include "terminal/TerminalFrontend.hpp"

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QObject>
#include <QTreeView>
#include <QLayout>
#include <QSplitter>
#include <QToolBar>
#include <QPushButton>

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
    openide::terminal::TerminalFrontend& getTerminalFrontend();
    void setProjectTitle(const QString& projectName);
    QString getCurrentProjectRoot() const { return m_currentProjectRoot; }
    ~MainWindow();

private slots:
    void onProjectOpened(const QString& projectPath, const QString& projectName);
    void toggleProjectTree();

private:
    Ui::MainWindow *ui;
    QWidget* m_centralWidget;
    QGridLayout* m_layout;
    QSplitter* m_horizontalSplitter;
    QSplitter* m_verticalSplitter;
    QToolBar* m_toolBar;
    QAction* m_toggleTreeAction;
    openide::ProjectTree m_projectTree;
    openide::code::CodeTabPane m_codeTabPane;
    openide::menu::FileMenu m_fileMenu;
    openide::menu::EditMenu m_editMenu;
    openide::menu::ThemeMenu m_themeMenu;
    openide::menu::SettingsMenu m_settingsMenu;
    openide::menu::TerminalMenu m_terminalMenu;
    openide::terminal::TerminalFrontend m_terminalFrontend;
    openide::AppSettings m_appSettings;
    QString m_currentProjectName;
    QString m_currentProjectRoot;
};

#endif // MAINWINDOW_HPP
