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
    , m_layout(new QGridLayout(m_centralWidget))
    , m_splitter(new QSplitter(Qt::Horizontal, m_centralWidget))
    , m_toolBar(nullptr)
    , m_toggleTreeAction(nullptr)
    , m_projectTree(this)
    , m_codeTabPane(this)
    , m_fileMenu(this, this->menuBar(), &m_projectTree, &m_codeTabPane)
    , m_editMenu(this, this->menuBar(), &m_codeTabPane)
    , m_themeMenu(this, this->menuBar())
    , m_settingsMenu(this, this->menuBar(), &m_appSettings)
{
    // Load settings on startup
    m_appSettings.loadFromFile();
    
    // Initialize window title
    setWindowTitle("openIDE");
    
    QMainWindow::resize(1200, 800);
    setCentralWidget(m_centralWidget);

    // Create toolbar with toggle button
    m_toolBar = addToolBar("Main Toolbar");
    m_toggleTreeAction = m_toolBar->addAction("Show Tree");
    m_toggleTreeAction->setToolTip("Show Project Tree (Ctrl+B)");
    m_toggleTreeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
    connect(m_toggleTreeAction, &QAction::triggered, this, &MainWindow::toggleProjectTree);

    // Setup splitter with ProjectTree and CodeTabPane
    // Note: Don't manually add widgets to layout in their constructors anymore
    // The splitter manages them now
    m_splitter->addWidget(&m_projectTree);
    m_splitter->addWidget(&m_codeTabPane);
    
    // Set initial sizes: ~20% for tree, ~80% for editor
    QList<int> sizes;
    sizes << 240 << 960;  // Based on 1200px width
    m_splitter->setSizes(sizes);
    
    // Make splitter handle more visible
    m_splitter->setHandleWidth(3);
    
    // Add splitter to layout
    m_layout->addWidget(m_splitter, 0, 0);
    m_layout->setContentsMargins(0, 0, 0, 0);

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
    
    // Update Edit menu state when tabs change
    connect(&m_codeTabPane, &openide::code::CodeTabPane::editMenuStateChanged, this, [this](){
        m_editMenu.updateFindActionState();
    });
    
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

void MainWindow::toggleProjectTree()
{
    bool isVisible = m_projectTree.isVisible();
    m_projectTree.setVisible(!isVisible);
    
    // Update button text based on state
    if (isVisible) {
        m_toggleTreeAction->setText("Show Tree");
        m_toggleTreeAction->setToolTip("Show Project Tree (Ctrl+B)");
    } else {
        m_toggleTreeAction->setText("Hide Tree");
        m_toggleTreeAction->setToolTip("Hide Project Tree (Ctrl+B)");
    }
}

MainWindow::~MainWindow()
{
    delete m_centralWidget;
    delete m_layout;
}
