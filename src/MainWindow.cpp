#include "MainWindow.hpp"
#include "ProjectTree.hpp"
#include "code/CodeTabPane.hpp"
#include "menu/ThemeMenu.hpp"
#include "menu/SettingsMenu.hpp"
#include "AppSettings.hpp"
#include "terminal/TerminalBackendInterface.hpp"
#ifdef WIN32
#include "terminal/WindowsTerminalBackend.hpp"
#endif
#include <QApplication>

using namespace openide;
using namespace openide::terminal;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(new QWidget(this))
    , m_layout(new QGridLayout(m_centralWidget))
    , m_horizontalSplitter(new QSplitter(Qt::Horizontal, m_centralWidget))
    , m_verticalSplitter(new QSplitter(Qt::Vertical, m_centralWidget))
    , m_toolBar(nullptr)
    , m_toggleTreeAction(nullptr)
    , m_projectTree(this)
    , m_codeTabPane(this)
    , m_fileMenu(this, this->menuBar(), &m_projectTree, &m_codeTabPane)
    , m_editMenu(this, this->menuBar(), &m_codeTabPane)
    , m_themeMenu(this, this->menuBar())
    , m_settingsMenu(this, this->menuBar(), &m_appSettings)
    , m_terminalMenu(this, this->menuBar())
    , m_terminalBackend(nullptr)
    , m_terminalFrontend(this, nullptr)
{
    // Load settings on startup
    m_appSettings.loadFromFile();
    
    // Create OS-specific terminal backend
#ifdef WIN32
    m_terminalBackend = new WindowsTerminalBackend();
#else
    // TODO: Add other OS backends (macOS, Linux) when implemented
    m_terminalBackend = nullptr;
#endif
    
    // Set the backend in the terminal frontend
    m_terminalFrontend.setBackend(m_terminalBackend);
    
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

    // Vertical splitter for the CodeTabPane and TerminalFrontend
    m_verticalSplitter->addWidget(&m_codeTabPane);
    m_verticalSplitter->addWidget(&m_terminalFrontend);
    m_verticalSplitter->setSizes({550, 250});
    m_verticalSplitter->setOpaqueResize(false);
    m_verticalSplitter->setHandleWidth(3);
    // Set initial style (will be updated when theme is applied)
    m_verticalSplitter->setStyleSheet("QSplitter::handle { background-color: #4a4a4a; }");
    
    // Hide terminal by default
    m_terminalFrontend.setVisible(false);
    
    // Horizontal splitter for ProjectTree and "Code Area"
    m_horizontalSplitter->addWidget(&m_projectTree);
    m_horizontalSplitter->addWidget(m_verticalSplitter);
    m_horizontalSplitter->setSizes({240, 960}); // Based on 1200px width
    m_horizontalSplitter->setHandleWidth(3);
    m_horizontalSplitter->setOpaqueResize(false);
    // Set initial style (will be updated when theme is applied)
    m_horizontalSplitter->setStyleSheet("QSplitter::handle { background-color: #4a4a4a; }");
    
    // Add splitter to layout
    m_layout->addWidget(m_horizontalSplitter, 0, 0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    /* CALLBACKS/HANDLER INITIALIZATION */

    // ProjectTree handles file opening internally now
    
    // Connect theme changes to update all code editors, terminal, and splitter styles
    // The themeChanged signal emits the actual theme (Light or Dark), not System
    connect(&m_themeMenu, &openide::menu::ThemeMenu::themeChanged, this, [this](openide::menu::ThemeMenu::Theme theme){
        bool isDark = (theme == openide::menu::ThemeMenu::Theme::Dark);
        m_codeTabPane.updateAllEditorsTheme(isDark);
        m_terminalFrontend.updateTheme(isDark);
        updateSplitterStyles(isDark);
        m_codeTabPane.updateAllSplitterStyles(isDark);
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

TerminalFrontend& MainWindow::getTerminalFrontend()
{
    return m_terminalFrontend;
}

void MainWindow::setComponentsVisible(bool isVisible)
{
    m_projectTree.setVisible(isVisible);
    m_codeTabPane.setComponentVisible(isVisible);
}

void MainWindow::updateSplitterStyles(bool isDarkTheme)
{
    QString handleStyle;
    if (isDarkTheme) {
        // Dark theme: lighter handle to distinguish from dark background (#353535)
        handleStyle = "QSplitter::handle { background-color: #4a4a4a; }";
    } else {
        // Light theme: slightly darker handle to distinguish from light background (#ffffff)
        handleStyle = "QSplitter::handle { background-color: #d0d0d0; }";
    }
    
    // Apply to main window splitters
    if (m_horizontalSplitter) {
        m_horizontalSplitter->setStyleSheet(handleStyle);
    }
    if (m_verticalSplitter) {
        m_verticalSplitter->setStyleSheet(handleStyle);
    }
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
    // Cleanup terminal backend
    if (m_terminalBackend) {
        delete m_terminalBackend;
        m_terminalBackend = nullptr;
    }
    
    delete m_centralWidget;
    delete m_layout;
}
