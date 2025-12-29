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
    , m_fileMenu(this, this->menuBar(), &m_projectTree,
        // saveFileCallback
        [this](){m_codeTabPane.saveActiveFile();},
         // saveAllFilesCallback
        [this](){m_codeTabPane.saveAllActiveFiles();})
    , m_themeMenu(this, this->menuBar())
    , m_settingsMenu(this, this->menuBar(), &m_appSettings)
{
    // Load settings on startup
    m_appSettings.loadFromFile();
    
    QMainWindow::resize(1200, 800);
    setCentralWidget(m_centralWidget);

    // grid styling
    m_layout->setRowStretch(0, 1); // make row 0 stretchable
    m_layout->setColumnStretch(0, 1); // column 0 ~20% of width
    m_layout->setColumnStretch(1, 4); // column 1 ~80% of width

    /* CALLBACKS/HANDLER INITIALIZATION */

    // double clicking file on project tree populates code editor
    // TODO: move this to the actual ProjectTree class ctor isntead of main window
    QObject::connect(m_projectTree.getTreeView(), &QTreeView::doubleClicked, this, [this](const QModelIndex& index){
        m_projectTree.onClick(m_codeTabPane, index, &m_appSettings);
    });
    
    // Connect theme changes to update all code editors
    // The themeChanged signal emits the actual theme (Light or Dark), not System
    connect(&m_themeMenu, &openide::menu::ThemeMenu::themeChanged, this, [this](openide::menu::Theme theme){
        bool isDark = (theme == openide::menu::Theme::Dark);
        m_codeTabPane.updateAllEditorsTheme(isDark);
    });
    
    // Connect settings changes to update all code editors
    connect(&m_settingsMenu, &openide::menu::SettingsMenu::settingsChanged, this, [this](){
        m_codeTabPane.updateAllEditorsSettings(&m_appSettings);
    });
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
    this->m_projectTree.setComponentVisible(true);
    this->m_codeTabPane.setComponentVisible(true);
}

MainWindow::~MainWindow()
{
    delete m_centralWidget;
    delete m_layout;
}
