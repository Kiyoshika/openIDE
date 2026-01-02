#include "code/PaneContainer.hpp"
#include "ui/StyleUtils.hpp"
#include "code/CodeEditor.hpp"
#include "MainWindow.hpp"

using namespace openide::code;

PaneContainer::PaneContainer(MainWindow* parent, Type type)
    : QWidget(parent)
    , m_parent(parent)
    , m_type(type)
    , m_layout(new QVBoxLayout(this))
    , m_tabWidget(nullptr)
    , m_paneLabel(nullptr)
    , m_splitter(nullptr)
    , m_leftChild(nullptr)
    , m_rightChild(nullptr)
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    if (m_type == Type::Leaf) {
        convertToLeaf();
    }
}

PaneContainer::~PaneContainer()
{
    // Children are automatically deleted by Qt parent-child relationship
}

void PaneContainer::convertToLeaf()
{
    // Clean up branch members if they exist
    if (m_leftChild) {
        m_leftChild->deleteLater();
        m_leftChild = nullptr;
    }
    if (m_rightChild) {
        m_rightChild->deleteLater();
        m_rightChild = nullptr;
    }
    if (m_splitter) {
        m_layout->removeWidget(m_splitter);
        m_splitter->deleteLater();
        m_splitter = nullptr;
    }

    // Create tab widget if it doesn't exist
    if (!m_tabWidget) {
        m_tabWidget = new QTabWidget(this);
        m_tabWidget->setTabsClosable(true);
        m_tabWidget->setMovable(true);
        m_tabWidget->setDocumentMode(true);
        m_layout->addWidget(m_tabWidget);
        
        emit tabWidgetCreated(m_tabWidget);
    }
    
    // Create pane label if it doesn't exist
    if (!m_paneLabel) {
        m_paneLabel = new QLabel(this);
        m_paneLabel->setAlignment(Qt::AlignCenter);
        m_paneLabel->setStyleSheet("QLabel { background-color: palette(window); color: palette(window-text); padding: 2px; font-size: 10px; }");
        m_paneLabel->setText("Pane 1");
        m_layout->addWidget(m_paneLabel);
    }

    m_type = Type::Leaf;
}

void PaneContainer::convertToBranch(Qt::Orientation orientation)
{
    if (m_type == Type::Branch) return; // Already a branch

    // Save existing tab widget content
    QTabWidget* oldTabWidget = m_tabWidget;
    m_tabWidget = nullptr;
    
    // Remove the old tab widget from layout
    if (oldTabWidget) {
        m_layout->removeWidget(oldTabWidget);
    }
    
    // Remove and delete the pane label
    if (m_paneLabel) {
        m_layout->removeWidget(m_paneLabel);
        m_paneLabel->deleteLater();
        m_paneLabel = nullptr;
    }

    // Create splitter
    m_splitter = new QSplitter(orientation, this);
    m_splitter->setHandleWidth(3);
    m_splitter->setOpaqueResize(false);
    // Set initial style based on current theme (will be updated when theme changes)
    // Default to dark theme style
    m_splitter->setStyleSheet(openide::ui::StyleUtils::getSplitterHandleStyle(true));
    m_layout->addWidget(m_splitter);

    // Create two child containers
    m_leftChild = new PaneContainer(m_parent, Type::Leaf);
    m_rightChild = new PaneContainer(m_parent, Type::Leaf);

    // Forward signals from children
    connect(m_leftChild, &PaneContainer::tabWidgetCreated, 
            this, &PaneContainer::tabWidgetCreated);
    connect(m_rightChild, &PaneContainer::tabWidgetCreated, 
            this, &PaneContainer::tabWidgetCreated);
    connect(m_leftChild, &PaneContainer::containerEmpty, 
            this, &PaneContainer::containerEmpty);
    connect(m_rightChild, &PaneContainer::containerEmpty, 
            this, &PaneContainer::containerEmpty);

    m_splitter->addWidget(m_leftChild);
    m_splitter->addWidget(m_rightChild);

    // Move all tabs from old tab widget to left child
    if (oldTabWidget) {
        while (oldTabWidget->count() > 0) {
            QWidget* widget = oldTabWidget->widget(0);
            QString text = oldTabWidget->tabText(0);
            oldTabWidget->removeTab(0);
            m_leftChild->addTab(widget, text);
        }
        oldTabWidget->deleteLater();
    }

    // Set equal sizes
    QList<int> sizes;
    sizes << 1 << 1;
    m_splitter->setSizes(sizes);

    m_type = Type::Branch;
}

void PaneContainer::splitHorizontal()
{
    // Horizontal split = horizontal divider = top/bottom panes
    convertToBranch(Qt::Vertical);
}

void PaneContainer::splitVertical()
{
    // Vertical split = vertical divider = left/right panes
    convertToBranch(Qt::Horizontal);
}

int PaneContainer::tabCount() const
{
    if (m_type == Type::Leaf && m_tabWidget) {
        return m_tabWidget->count();
    }
    return 0;
}

void PaneContainer::addTab(QWidget* widget, const QString& label)
{
    if (m_type == Type::Leaf && m_tabWidget) {
        m_tabWidget->addTab(widget, label);
    }
}

void PaneContainer::removeTab(int index)
{
    if (m_type == Type::Leaf && m_tabWidget) {
        m_tabWidget->removeTab(index);
        
        // Notify if empty
        if (m_tabWidget->count() == 0) {
            emit containerEmpty(this);
        }
    }
}

QWidget* PaneContainer::widget(int index) const
{
    if (m_type == Type::Leaf && m_tabWidget) {
        return m_tabWidget->widget(index);
    }
    return nullptr;
}

QString PaneContainer::tabText(int index) const
{
    if (m_type == Type::Leaf && m_tabWidget) {
        return m_tabWidget->tabText(index);
    }
    return QString();
}

void PaneContainer::setTabText(int index, const QString& text)
{
    if (m_type == Type::Leaf && m_tabWidget) {
        m_tabWidget->setTabText(index, text);
    }
}

int PaneContainer::currentIndex() const
{
    if (m_type == Type::Leaf && m_tabWidget) {
        return m_tabWidget->currentIndex();
    }
    return -1;
}

void PaneContainer::setCurrentIndex(int index)
{
    if (m_type == Type::Leaf && m_tabWidget) {
        m_tabWidget->setCurrentIndex(index);
    }
}

QWidget* PaneContainer::currentWidget() const
{
    if (m_type == Type::Leaf && m_tabWidget) {
        return m_tabWidget->currentWidget();
    }
    return nullptr;
}

void PaneContainer::getAllTabWidgets(QList<QTabWidget*>& widgets)
{
    if (m_type == Type::Leaf && m_tabWidget) {
        widgets.append(m_tabWidget);
    } else if (m_type == Type::Branch) {
        if (m_leftChild) m_leftChild->getAllTabWidgets(widgets);
        if (m_rightChild) m_rightChild->getAllTabWidgets(widgets);
    }
}

void PaneContainer::getAllLeafContainers(QList<PaneContainer*>& containers)
{
    if (m_type == Type::Leaf) {
        containers.append(this);
    } else if (m_type == Type::Branch) {
        if (m_leftChild) m_leftChild->getAllLeafContainers(containers);
        if (m_rightChild) m_rightChild->getAllLeafContainers(containers);
    }
}

PaneContainer* PaneContainer::findContainerWithTabWidget(QTabWidget* tabWidget)
{
    if (m_type == Type::Leaf && m_tabWidget == tabWidget) {
        return this;
    } else if (m_type == Type::Branch) {
        if (m_leftChild) {
            PaneContainer* found = m_leftChild->findContainerWithTabWidget(tabWidget);
            if (found) return found;
        }
        if (m_rightChild) {
            PaneContainer* found = m_rightChild->findContainerWithTabWidget(tabWidget);
            if (found) return found;
        }
    }
    return nullptr;
}

bool PaneContainer::isEmpty() const
{
    if (m_type == Type::Leaf) {
        return !m_tabWidget || m_tabWidget->count() == 0;
    } else if (m_type == Type::Branch) {
        return (!m_leftChild || m_leftChild->isEmpty()) && 
               (!m_rightChild || m_rightChild->isEmpty());
    }
    return true;
}

void PaneContainer::setPaneNumber(int number)
{
    if (m_type == Type::Leaf && m_paneLabel) {
        m_paneLabel->setText(QString("Pane %1").arg(number));
    }
}

void PaneContainer::simplify()
{
    if (m_type != Type::Branch) return;

    // First, recursively simplify children
    if (m_leftChild) m_leftChild->simplify();
    if (m_rightChild) m_rightChild->simplify();

    bool leftEmpty = !m_leftChild || m_leftChild->isEmpty();
    bool rightEmpty = !m_rightChild || m_rightChild->isEmpty();

    // If both children are empty, convert to empty leaf
    if (leftEmpty && rightEmpty) {
        convertToLeaf();
        return;
    }

    // If left is empty but right has content, replace this with right's content
    if (leftEmpty && !rightEmpty) {
        if (m_rightChild->isLeaf()) {
            // Move tabs from right child to this container as a leaf
            QTabWidget* rightTabs = m_rightChild->tabWidget();
            convertToLeaf();
            while (rightTabs && rightTabs->count() > 0) {
                QWidget* widget = rightTabs->widget(0);
                QString text = rightTabs->tabText(0);
                rightTabs->removeTab(0);
                addTab(widget, text);
            }
        } else {
            // Right is a branch, promote its structure to this level
            QSplitter* rightSplitter = m_rightChild->splitter();
            PaneContainer* rightLeft = m_rightChild->leftChild();
            PaneContainer* rightRight = m_rightChild->rightChild();
            
            // Steal right's children
            m_rightChild->m_leftChild = nullptr;
            m_rightChild->m_rightChild = nullptr;
            
            // Remove current children
            m_layout->removeWidget(m_splitter);
            m_leftChild->deleteLater();
            m_rightChild->deleteLater();
            m_splitter->deleteLater();
            
            // Adopt right's structure
            m_splitter = new QSplitter(rightSplitter->orientation(), this);
            m_layout->addWidget(m_splitter);
            m_leftChild = rightLeft;
            m_rightChild = rightRight;
            m_leftChild->setParent(this);
            m_rightChild->setParent(this);
            m_splitter->addWidget(m_leftChild);
            m_splitter->addWidget(m_rightChild);
        }
        return;
    }

    // If right is empty but left has content, replace this with left's content
    if (rightEmpty && !leftEmpty) {
        if (m_leftChild->isLeaf()) {
            // Move tabs from left child to this container as a leaf
            QTabWidget* leftTabs = m_leftChild->tabWidget();
            convertToLeaf();
            while (leftTabs && leftTabs->count() > 0) {
                QWidget* widget = leftTabs->widget(0);
                QString text = leftTabs->tabText(0);
                leftTabs->removeTab(0);
                addTab(widget, text);
            }
        } else {
            // Left is a branch, promote its structure to this level
            QSplitter* leftSplitter = m_leftChild->splitter();
            PaneContainer* leftLeft = m_leftChild->leftChild();
            PaneContainer* leftRight = m_leftChild->rightChild();
            
            // Steal left's children
            m_leftChild->m_leftChild = nullptr;
            m_leftChild->m_rightChild = nullptr;
            
            // Remove current children
            m_layout->removeWidget(m_splitter);
            m_leftChild->deleteLater();
            m_rightChild->deleteLater();
            m_splitter->deleteLater();
            
            // Adopt left's structure
            m_splitter = new QSplitter(leftSplitter->orientation(), this);
            m_layout->addWidget(m_splitter);
            m_leftChild = leftLeft;
            m_rightChild = leftRight;
            m_leftChild->setParent(this);
            m_rightChild->setParent(this);
            m_splitter->addWidget(m_leftChild);
            m_splitter->addWidget(m_rightChild);
        }
        return;
    }
}

