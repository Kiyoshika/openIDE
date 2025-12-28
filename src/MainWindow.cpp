#include "MainWindow.hpp"
#include "ProjectTree.hpp"
#include "code/CodeTabPane.hpp"

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
{
    QMainWindow::resize(1200, 800);
    setCentralWidget(m_centralWidget);

    // grid styling
    m_layout->setRowStretch(0, 1); // make row 0 stretchable
    m_layout->setColumnStretch(0, 1); // column 0 ~20% of width
    m_layout->setColumnStretch(1, 4); // column 1 ~80% of width

    /* CALLBACKS/HANDLER INITIALIZATION */

    // double clicking file on project tree populates code editor
    QObject::connect(m_projectTree.getTreeView(), &QTreeView::doubleClicked, this, [this](const QModelIndex& index){
        m_projectTree.onClick(m_codeTabPane, index);
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

void MainWindow::initFileMenu(QMenuBar* menuBar)
{
    QMenu* fileMenu = menuBar->addMenu("&File");
    QAction* newAction = new QAction("&New Project", this);
    QAction* openAction = new QAction("&Open Project", this);

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);

    QObject::connect(openAction, &QAction::triggered, this, [this](){
        QString filePath;
        FileMenu::openDir(this, &filePath);
        if (!filePath.isEmpty())
            m_projectTree.loadTreeFromDir(&filePath);
    });
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
