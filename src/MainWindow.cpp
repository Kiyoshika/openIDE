#include "MainWindow.hpp"
#include "ProjectTree.hpp"
#include "code/CodeTabPane.hpp"
#include "menu/ThemeMenu.hpp"
#include "menu/SettingsMenu.hpp"
#include "AppSettings.hpp"
#include <QApplication>

using namespace openide;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(new QWidget(this))
    // right now, hardlocking to grid layout - too much complexity
    // for what it's worth to make this super generic because then
    // you'd need a separate implementation for every widget added
    // for that specified layout type.
    , m_layout(new QGridLayout(m_centralWidget))
    , m_projectTree(this)
    , m_codeTabPane(this)
    , m_fileMenu(this, this->menuBar(), &m_projectTree, &m_codeTabPane)
    , m_themeMenu(this, this->menuBar())
    , m_settingsMenu(this, this->menuBar(), &m_appSettings)
{
    // Load settings on startup
    m_appSettings.loadFromFile();
    
    // Initialize window title
    setWindowTitle("openIDE");
    
    QMainWindow::resize(1200, 800);
    setCentralWidget(m_centralWidget);

    // grid styling
    m_layout->setRowStretch(0, 1); // make row 0 stretchable
    m_layout->setColumnStretch(0, 1); // column 0 ~20% of width
    m_layout->setColumnStretch(1, 4); // column 1 ~80% of width

    /* CALLBACKS/HANDLER INITIALIZATION */

    // ProjectTree handles file opening internally now
    
    // Connect theme changes to update all code editors
    // The themeChanged signal emits the actual theme (Light or Dark), not System
    connect(&m_themeMenu, &openide::menu::ThemeMenu::themeChanged, this, [this](openide::menu::ThemeMenu::Theme theme){
        bool isDark = (theme == openide::menu::ThemeMenu::Theme::Dark);
        m_codeTabPane.updateAllEditorsTheme(isDark);
    });
    
    // Connect settings changes to update all code editors
    connect(&m_settingsMenu, &openide::menu::SettingsMenu::settingsChanged, this, [this](){
        m_codeTabPane.updateAllEditorsSettings(&m_appSettings);
    });
    
    // Connect project opened signal to update title
    connect(&m_fileMenu, &openide::menu::FileMenu::projectOpened, this, &MainWindow::onProjectOpened);
    
    // Now that all connections are set up, apply the initial theme
    m_themeMenu.applyTheme(openide::menu::ThemeMenu::Theme::System);
}

void MainWindow::onProjectOpened(const QString& projectPath, const QString& projectName)
{
    // Normalize the path to ensure it's stored correctly
    QDir dir(projectPath);
    m_currentProjectRoot = dir.absolutePath();
    setProjectTitle(projectName);
}

void MainWindow::setProjectTitle(const QString& projectName)
{
    m_currentProjectName = projectName;
    if (projectName.isEmpty()) {
        setWindowTitle("openIDE");
    } else {
        setWindowTitle("openIDE - " + projectName);
    }
}

QWidget* MainWindow::getCentralWidget() const
{
    return m_centralWidget;
}

QGridLayout* MainWindow::getLayout() const
{
    return m_layout;
}

void MainWindow::setComponentsVisible(bool isVisible)
{
    m_projectTree.setVisible(isVisible);
    m_codeTabPane.setComponentVisible(isVisible);
}

MainWindow::~MainWindow()
{
    delete m_centralWidget;
    delete m_layout;
}
