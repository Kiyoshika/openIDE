#include "MainWindow.hpp"
#include "ProjectTree.hpp"
#include "./ui_MainWindow.h"

using namespace openide;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
}

void MainWindow::initLayout()
{
    // right now, hardlocking to grid layout - too much complexity
    // for what it's worth to make this super generic because then
    // you'd need a separate implementation for every widget added
    // for that specified layout type.
    m_layout = new QGridLayout(m_centralWidget);
}

void MainWindow::initProjectTree()
{
    m_projectTree = new QTreeView(m_centralWidget);
    m_projectTree->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_layout->addWidget(m_projectTree, 0, 0);
    m_layout->setRowStretch(0, 1); // make row 0 stretchable
    m_layout->setColumnStretch(0, 1); // column 0 ~20% of width
    m_layout->setColumnStretch(1, 4); // column 1 ~80% of width
}

void MainWindow::initFileMenu(QMenuBar* menuBar)
{
    QMenu* fileMenu = menuBar->addMenu("&File");
    QAction* newAction = new QAction("&New Project", this);
    QAction* openAction = new QAction("&Open Project", this);

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);

    QString filePath;
    QObject::connect(openAction, &QAction::triggered, this, [this, &filePath](){
        filemenu::openDir(this, &filePath);
        if (!filePath.isEmpty())
            projecttree::loadTreeFromDir(this, &filePath, m_projectTree);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_centralWidget;
    delete m_layout;
    delete m_projectTree;
}
