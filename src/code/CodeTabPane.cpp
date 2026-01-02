#include "code/CodeTabPane.hpp"
#include "code/CodeEditor.hpp"
#include "code/PaneContainer.hpp"
#include "MainWindow.hpp"
#include "AppSettings.hpp"
#include "FileType.hpp"
#include <QTabBar>
#include <QMouseEvent>
#include <QFileInfo>

using namespace openide::code;

CodeTabPane::CodeTabPane(MainWindow *parent)
    : QWidget(parent ? parent->getCentralWidget() : parent)
    , m_parent(parent)
    , m_layout(new QVBoxLayout(this))
    , m_root(nullptr)
    , m_activeTabWidget(nullptr)
    , m_contextMenuTabIndex(-1)
    , m_contextMenuPane(nullptr)
    , m_dragSourcePane(nullptr)
    , m_dragSourceIndex(-1)
{
    // Create root container (starts as a leaf with one tab widget)
    m_root = new PaneContainer(m_parent, PaneContainer::Type::Leaf);
    
    // Setup layout
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_root);
    setLayout(m_layout);
    
    // Setup the initial tab widget
    m_activeTabWidget = m_root->tabWidget();
    setupTabWidget(m_activeTabWidget);
    
    // Install event filter on self to track focus
    installEventFilter(this);
    
    // Connect to signals from root container
    connect(m_root, &PaneContainer::tabWidgetCreated, this, [this](QTabWidget* tabWidget){
        setupTabWidget(tabWidget);
    });
    
    connect(m_root, &PaneContainer::containerEmpty, this, [this](PaneContainer*){
        // Simplify the tree structure when a container becomes empty
        m_root->simplify();
        
        // Update active tab widget
        QTabWidget* newActive = getActiveTabWidget();
        if (newActive) {
            m_activeTabWidget = newActive;
        }
        
        // Update pane numbers
        updatePaneNumbers();
    });
    
    // Initially hide (will be shown when project opens)
    setVisible(false);
    
    // Initialize pane numbers
    updatePaneNumbers();
    
    // create shortcut to save files
    QAction* saveAction = new QAction("&Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setShortcutContext(Qt::WindowShortcut);
    saveAction->setEnabled(true);
    addAction(saveAction);
    
    // save file in current tab handler
    connect(saveAction, &QAction::triggered, this, [this] {
        saveActiveFile();
    });
}

void CodeTabPane::setupTabWidget(QTabWidget* tabWidget)
{
    if (!tabWidget) return;
    
    // Check if already set up (to avoid duplicate connections)
    if (tabWidget->property("openide_setup").toBool()) {
        return;
    }
    tabWidget->setProperty("openide_setup", true);
    
    // Enable accepting drops for cross-pane dragging
    tabWidget->setAcceptDrops(true);
    tabWidget->tabBar()->setAcceptDrops(true);
    
    // Allow tabs to be moved between tab widgets
    tabWidget->tabBar()->setChangeCurrentOnDrag(true);
    
    // Install event filter for context menu and drag tracking
    tabWidget->tabBar()->installEventFilter(this);
    tabWidget->installEventFilter(this);
    
    // Handler to close the tab
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, [this, tabWidget](int index){
        closeTab(tabWidget, index);
    });
    
    // Track active tab widget and update edit menu
    connect(tabWidget, &QTabWidget::currentChanged, this, [this, tabWidget](int){
        m_activeTabWidget = tabWidget;
        emit editMenuStateChanged();
    });
    
    // Track when tab bar is clicked or moved
    connect(tabWidget->tabBar(), &QTabBar::tabMoved, this, [this, tabWidget](int, int){
        m_activeTabWidget = tabWidget;
    });
    
    connect(tabWidget->tabBar(), &QTabBar::currentChanged, this, [this, tabWidget](int){
        m_activeTabWidget = tabWidget;
    });
}

void CodeTabPane::connectEditorSignals(CodeEditor* editor, QTabWidget* tabWidget)
{
    if (!editor) return;
    
    // Disconnect any existing fileModified connections to prevent duplicates
    disconnect(editor, &CodeEditor::fileModified, this, nullptr);
    
    // Connect fileModified signal to mark the tab as dirty
    // Dynamically find the tabWidget containing this editor instead of capturing it
    // This makes the connection resilient to tab moves and pane restructuring
    connect(editor, &CodeEditor::fileModified, this, [this, editor]() {
        if (!editor || !m_root) return;
        
        // Find which tabWidget contains this editor by searching all tab widgets
        QList<QTabWidget*> allTabWidgets;
        m_root->getAllTabWidgets(allTabWidgets);
        
        for (QTabWidget* tw : allTabWidgets) {
            if (!tw) continue;
            
            // Find the tab index for this editor
            for (int i = 0; i < tw->count(); ++i) {
                if (tw->widget(i) == editor) {
                    QString tabText = tw->tabText(i);
                    // Only add asterisk if not already present
                    if (!tabText.endsWith(" *")) {
                        tw->setTabText(i, tabText + " *");
                    }
                    return; // Found and updated, exit early
                }
            }
        }
    });
}

PaneContainer* CodeTabPane::findContainerForTabWidget(QTabWidget* tabWidget)
{
    return m_root ? m_root->findContainerWithTabWidget(tabWidget) : nullptr;
}

QTabWidget* CodeTabPane::getActiveTabWidget()
{
    // If we have a tracked active, return it
    if (m_activeTabWidget) {
        // Verify it still exists in the tree
        if (findContainerForTabWidget(m_activeTabWidget)) {
            return m_activeTabWidget;
        }
    }
    
    // Otherwise, return the first non-empty tab widget
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(allTabWidgets);
    }
    
    for (QTabWidget* tw : allTabWidgets) {
        if (tw->count() > 0) {
            m_activeTabWidget = tw;
            return tw;
        }
    }
    
    // If no non-empty widgets, return any widget
    if (!allTabWidgets.isEmpty()) {
        m_activeTabWidget = allTabWidgets.first();
        return m_activeTabWidget;
    }
    
    return nullptr;
}

void CodeTabPane::updatePaneNumbers()
{
    if (!m_root) return;
    
    // Get all leaf containers
    QList<PaneContainer*> leafContainers;
    m_root->getAllLeafContainers(leafContainers);
    
    // Assign numbers to each pane
    for (int i = 0; i < leafContainers.size(); ++i) {
        leafContainers[i]->setPaneNumber(i + 1);
    }
}

void CodeTabPane::addTab(CodeEditor *editor, const QString &tabName)
{
    if (!editor || !m_root) return;
    
    // Get the active tab widget (or create one if none exists)
    QTabWidget* targetWidget = getActiveTabWidget();
    
    if (!targetWidget) {
        // This shouldn't happen, but handle it gracefully
        if (m_root->isLeaf()) {
            targetWidget = m_root->tabWidget();
        }
    }
    
    if (targetWidget) {
        // Add tab to the target widget
        int index = targetWidget->addTab(editor, tabName);
        targetWidget->setCurrentIndex(index);
        m_activeTabWidget = targetWidget;
        
        // Install event filter on the editor to track focus
        editor->installEventFilter(this);
        
        // Connect editor signals to update tab state
        connectEditorSignals(editor, targetWidget);
    }
}

void CodeTabPane::setComponentVisible(bool isVisible)
{
    setVisible(isVisible);
}

int CodeTabPane::count() const
{
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(const_cast<QList<QTabWidget*>&>(allTabWidgets));
    }
    
    int total = 0;
    for (QTabWidget* tw : allTabWidgets) {
        total += tw->count();
    }
    return total;
}

QWidget* CodeTabPane::widget(int index) const
{
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(const_cast<QList<QTabWidget*>&>(allTabWidgets));
    }
    
    int currentOffset = 0;
    for (QTabWidget* tw : allTabWidgets) {
        if (index < currentOffset + tw->count()) {
            return tw->widget(index - currentOffset);
        }
        currentOffset += tw->count();
    }
    return nullptr;
}

int CodeTabPane::currentIndex() const
{
    QTabWidget* active = const_cast<CodeTabPane*>(this)->getActiveTabWidget();
    if (!active) return -1;
    
    // Calculate global index
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(const_cast<QList<QTabWidget*>&>(allTabWidgets));
    }
    
    int offset = 0;
    for (QTabWidget* tw : allTabWidgets) {
        if (tw == active) {
            return offset + tw->currentIndex();
        }
        offset += tw->count();
    }
    return -1;
}

QWidget* CodeTabPane::currentWidget() const
{
    QTabWidget* active = const_cast<CodeTabPane*>(this)->getActiveTabWidget();
    return active ? active->currentWidget() : nullptr;
}

void CodeTabPane::setTabText(int index, const QString& text)
{
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(allTabWidgets);
    }
    
    int currentOffset = 0;
    for (QTabWidget* tw : allTabWidgets) {
        if (index < currentOffset + tw->count()) {
            tw->setTabText(index - currentOffset, text);
            return;
        }
        currentOffset += tw->count();
    }
}

bool CodeTabPane::eventFilter(QObject* obj, QEvent* event)
{
    // Get all tab widgets
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(allTabWidgets);
    }
    
    // Check if this is a tab bar
    QTabBar* tabBar = qobject_cast<QTabBar*>(obj);
    if (tabBar) {
        // Find which tab widget this tab bar belongs to
        QTabWidget* pane = nullptr;
        for (QTabWidget* tw : allTabWidgets) {
            if (tw->tabBar() == tabBar) {
                pane = tw;
                break;
            }
        }
        
        if (pane) {
            // Track clicks on tab bar to update active pane
            if (event->type() == QEvent::MouseButtonPress || 
                event->type() == QEvent::MouseButtonRelease) {
                m_activeTabWidget = pane;
            }
            
            if (event->type() == QEvent::ContextMenu) {
                QContextMenuEvent* contextEvent = static_cast<QContextMenuEvent*>(event);
                int tabIndex = tabBar->tabAt(contextEvent->pos());
                if (tabIndex >= 0) {
                    m_activeTabWidget = pane;
                    showTabContextMenu(contextEvent->globalPos(), pane, tabIndex);
                    return true;
                }
            }
            else if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::LeftButton) {
                    int tabIndex = tabBar->tabAt(mouseEvent->pos());
                    if (tabIndex >= 0) {
                        m_dragSourcePane = pane;
                        m_dragSourceIndex = tabIndex;
                    }
                }
            }
        }
    }
    
    // Check if this is a tab widget
    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(obj);
    if (tabWidget) {
        for (QTabWidget* tw : allTabWidgets) {
            if (tw == tabWidget) {
                // Track focus and clicks on tab widget
                if (event->type() == QEvent::FocusIn || 
                    event->type() == QEvent::MouseButtonPress) {
                    m_activeTabWidget = tabWidget;
                }
                break;
            }
        }
    }
    
    // Check if this is a CodeEditor - track which pane it belongs to
    CodeEditor* editor = qobject_cast<CodeEditor*>(obj);
    if (editor && (event->type() == QEvent::FocusIn || 
                   event->type() == QEvent::MouseButtonPress)) {
        // Find which tab widget contains this editor
        for (QTabWidget* tw : allTabWidgets) {
            for (int i = 0; i < tw->count(); ++i) {
                if (tw->widget(i) == editor) {
                    m_activeTabWidget = tw;
                    return QWidget::eventFilter(obj, event);
                }
            }
        }
    }
    
    return QWidget::eventFilter(obj, event);
}

void CodeTabPane::showTabContextMenu(const QPoint& pos, QTabWidget* pane, int tabIndex)
{
    m_contextMenuPane = pane;
    m_contextMenuTabIndex = tabIndex;
    
    QMenu menu(this);
    
    QAction* closeAction = menu.addAction("Close");
    connect(closeAction, &QAction::triggered, this, [this]() {
        closeTab(m_contextMenuPane, m_contextMenuTabIndex);
    });
    
    QAction* closeOthersAction = menu.addAction("Close All Others");
    connect(closeOthersAction, &QAction::triggered, this, [this]() {
        closeAllOtherTabs(m_contextMenuPane, m_contextMenuTabIndex);
    });
    
    QAction* closeAllAction = menu.addAction("Close All");
    connect(closeAllAction, &QAction::triggered, this, [this]() {
        closeAllTabsInPane(m_contextMenuPane);
    });
    
    menu.addSeparator();
    
    // Add "Move to Other Pane" submenu if there are other panes
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(allTabWidgets);
    }
    
    if (allTabWidgets.size() > 1) {
        QMenu* moveMenu = menu.addMenu("Move to Pane");
        
        // Get all leaf containers to show their pane numbers
        QList<PaneContainer*> leafContainers;
        if (m_root) {
            m_root->getAllLeafContainers(leafContainers);
        }
        
        for (int i = 0; i < leafContainers.size(); ++i) {
            PaneContainer* container = leafContainers[i];
            QTabWidget* tw = container->tabWidget();
            if (tw && tw != pane) {
                QAction* moveAction = moveMenu->addAction(QString("Pane %1").arg(i + 1));
                connect(moveAction, &QAction::triggered, this, [this, tw]() {
                    moveTabToPane(m_contextMenuPane, m_contextMenuTabIndex, tw);
                });
            }
        }
        menu.addSeparator();
    }
    
    QAction* splitHAction = menu.addAction("Split Horizontal");
    connect(splitHAction, &QAction::triggered, this, [this]() {
        splitHorizontal(m_contextMenuPane, m_contextMenuTabIndex);
    });
    
    QAction* splitVAction = menu.addAction("Split Vertical");
    connect(splitVAction, &QAction::triggered, this, [this]() {
        splitVertical(m_contextMenuPane, m_contextMenuTabIndex);
    });
    
    menu.exec(pos);
}

void CodeTabPane::closeTab(QTabWidget* pane, int tabIndex)
{
    if (!pane || tabIndex < 0 || tabIndex >= pane->count()) return;
    
    QWidget* widget = pane->widget(tabIndex);
    pane->removeTab(tabIndex);
    
    if (widget) {
        widget->deleteLater();
    }
    
    // Check if pane is now empty and simplify if needed
    if (pane->count() == 0) {
        m_root->simplify();
        
        // Update active tab widget
        QTabWidget* newActive = getActiveTabWidget();
        if (newActive) {
            m_activeTabWidget = newActive;
        }
        
        // Update pane numbers
        updatePaneNumbers();
    }
}

void CodeTabPane::closeAllOtherTabs(QTabWidget* pane, int tabIndex)
{
    if (!pane || tabIndex < 0 || tabIndex >= pane->count()) return;
    
    // Save the widget we want to keep
    QWidget* keepWidget = pane->widget(tabIndex);
    QString keepText = pane->tabText(tabIndex);
    
    // Close all other tabs (iterate backwards to avoid index issues)
    for (int i = pane->count() - 1; i >= 0; --i) {
        if (i != tabIndex) {
            QWidget* widget = pane->widget(i);
            pane->removeTab(i);
            if (widget) {
                widget->deleteLater();
            }
        }
    }
}

void CodeTabPane::closeAllTabsInPane(QTabWidget* pane)
{
    if (!pane) return;
    
    while (pane->count() > 0) {
        QWidget* widget = pane->widget(0);
        pane->removeTab(0);
        if (widget) {
            widget->deleteLater();
        }
    }
    
    // Check if pane is now empty and simplify if needed
    if (pane->count() == 0) {
        m_root->simplify();
        
        // Update active tab widget
        QTabWidget* newActive = getActiveTabWidget();
        if (newActive) {
            m_activeTabWidget = newActive;
        }
        
        // Update pane numbers
        updatePaneNumbers();
    }
}

void CodeTabPane::closeAllTabs()
{
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(allTabWidgets);
    }
    
    for (QTabWidget* tw : allTabWidgets) {
        while (tw->count() > 0) {
            QWidget* widget = tw->widget(0);
            tw->removeTab(0);
            if (widget) {
                widget->deleteLater();
            }
        }
    }
    
    // Simplify the tree since all panes are now empty
    if (m_root) {
        m_root->simplify();
    }
    
    m_activeTabWidget = nullptr;
    
    // Update pane numbers
    updatePaneNumbers();
}

void CodeTabPane::moveTabToPane(QTabWidget* sourcePane, int tabIndex, QTabWidget* targetPane)
{
    if (!sourcePane || !targetPane || sourcePane == targetPane) return;
    if (tabIndex < 0 || tabIndex >= sourcePane->count()) return;
    
    // Get tab data
    QWidget* widget = sourcePane->widget(tabIndex);
    QString tabText = sourcePane->tabText(tabIndex);
    CodeEditor* editor = qobject_cast<CodeEditor*>(widget);
    
    // Remove from source (doesn't delete the widget)
    sourcePane->removeTab(tabIndex);
    
    // Add to target
    int newIndex = targetPane->addTab(widget, tabText);
    targetPane->setCurrentIndex(newIndex);
    m_activeTabWidget = targetPane;
    
    // Reconnect signals for the moved editor
    if (editor) {
        connectEditorSignals(editor, targetPane);
    }
    
    // Check if source pane is now empty and simplify if needed
    if (sourcePane->count() == 0) {
        m_root->simplify();
        
        // Update active tab widget
        QTabWidget* newActive = getActiveTabWidget();
        if (newActive) {
            m_activeTabWidget = newActive;
        }
        
        // Update pane numbers
        updatePaneNumbers();
    }
}

void CodeTabPane::splitHorizontal(QTabWidget* pane, int tabIndex)
{
    if (!pane || tabIndex < 0 || tabIndex >= pane->count()) return;
    
    // Find the container that owns this tab widget
    PaneContainer* container = findContainerForTabWidget(pane);
    if (!container || !container->isLeaf()) return;
    
    // Get the editor to duplicate
    CodeEditor* sourceEditor = qobject_cast<CodeEditor*>(pane->widget(tabIndex));
    if (!sourceEditor) return;
    
    // Split the container horizontally
    container->splitHorizontal();
    
    // Setup tab widgets for the new children (signal might have been missed)
    if (container->leftChild() && container->leftChild()->tabWidget()) {
        setupTabWidget(container->leftChild()->tabWidget());
    }
    if (container->rightChild() && container->rightChild()->tabWidget()) {
        setupTabWidget(container->rightChild()->tabWidget());
    }
    
    // Duplicate the editor to the right child
    CodeEditor* newEditor = duplicateEditor(sourceEditor);
    if (newEditor && container->rightChild() && container->rightChild()->tabWidget()) {
        QTabWidget* rightTabWidget = container->rightChild()->tabWidget();
        container->rightChild()->addTab(newEditor, sourceEditor->getFilePath());
        m_activeTabWidget = rightTabWidget;
        
        // Connect signals for the new editor
        connectEditorSignals(newEditor, rightTabWidget);
    }
    
    // Update pane numbers after splitting
    updatePaneNumbers();
}

void CodeTabPane::splitVertical(QTabWidget* pane, int tabIndex)
{
    if (!pane || tabIndex < 0 || tabIndex >= pane->count()) return;
    
    // Find the container that owns this tab widget
    PaneContainer* container = findContainerForTabWidget(pane);
    if (!container || !container->isLeaf()) return;
    
    // Get the editor to duplicate
    CodeEditor* sourceEditor = qobject_cast<CodeEditor*>(pane->widget(tabIndex));
    if (!sourceEditor) return;
    
    // Split the container vertically
    container->splitVertical();
    
    // Setup tab widgets for the new children (signal might have been missed)
    if (container->leftChild() && container->leftChild()->tabWidget()) {
        setupTabWidget(container->leftChild()->tabWidget());
    }
    if (container->rightChild() && container->rightChild()->tabWidget()) {
        setupTabWidget(container->rightChild()->tabWidget());
    }
    
    // Duplicate the editor to the right child
    CodeEditor* newEditor = duplicateEditor(sourceEditor);
    if (newEditor && container->rightChild() && container->rightChild()->tabWidget()) {
        QTabWidget* rightTabWidget = container->rightChild()->tabWidget();
        container->rightChild()->addTab(newEditor, sourceEditor->getFilePath());
        m_activeTabWidget = rightTabWidget;
        
        // Connect signals for the new editor
        connectEditorSignals(newEditor, rightTabWidget);
    }
    
    // Update pane numbers after splitting
    updatePaneNumbers();
}

CodeEditor* CodeTabPane::duplicateEditor(CodeEditor* source)
{
    if (!source) return nullptr;
    
    // Create new editor with same settings
    CodeEditor* newEditor = new CodeEditor(m_parent, m_parent->getAppSettings());
    
    // Get the file path and determine its type
    QString filePath = source->getFilePath();
    QFileInfo fileInfo(filePath);
    openide::FileType fileType = openide::FileTypeUtil::fromExtension(fileInfo.suffix());
    
    // Load the same file
    newEditor->loadFile(filePath, fileType);
    
    // Install event filter to track focus
    newEditor->installEventFilter(this);
    
    return newEditor;
}

void CodeTabPane::dragEnterEvent(QDragEnterEvent* event)
{
    // Accept tab drags from our tab bars
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(allTabWidgets);
    }
    
    for (QTabWidget* tw : allTabWidgets) {
        if (event->source() == tw->tabBar()) {
            event->acceptProposedAction();
            return;
        }
    }
}

void CodeTabPane::dragMoveEvent(QDragMoveEvent* event)
{
    // Always accept the drag move event
    event->acceptProposedAction();
}

void CodeTabPane::dropEvent(QDropEvent* event)
{
    if (!m_dragSourcePane || m_dragSourceIndex < 0 || m_dragSourceIndex >= m_dragSourcePane->count()) {
        event->ignore();
        m_dragSourcePane = nullptr;
        m_dragSourceIndex = -1;
        return;
    }
    
    // Find target tab widget at the drop position
    QPoint localPos = event->position().toPoint();
    QPoint globalPos = mapToGlobal(localPos);
    QTabWidget* targetPane = nullptr;
    
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(allTabWidgets);
    }
    
    // Check each tab widget to see if the drop is over it
    for (QTabWidget* tw : allTabWidgets) {
        QRect globalRect = QRect(tw->mapToGlobal(QPoint(0, 0)), tw->size());
        if (globalRect.contains(globalPos)) {
            targetPane = tw;
            break;
        }
    }
    
    // If dropping on a different pane, move the tab
    if (targetPane && targetPane != m_dragSourcePane) {
        // Get tab data before removing
        QWidget* widget = m_dragSourcePane->widget(m_dragSourceIndex);
        QString tabText = m_dragSourcePane->tabText(m_dragSourceIndex);
        CodeEditor* editor = qobject_cast<CodeEditor*>(widget);
        
        // Remove from source (this doesn't delete the widget)
        m_dragSourcePane->removeTab(m_dragSourceIndex);
        
        // Add to target
        int newIndex = targetPane->addTab(widget, tabText);
        targetPane->setCurrentIndex(newIndex);
        m_activeTabWidget = targetPane;
        
        // Reconnect signals for the moved editor
        if (editor) {
            connectEditorSignals(editor, targetPane);
        }
        
        // Check if source pane is now empty and simplify if needed
        if (m_dragSourcePane->count() == 0) {
            m_root->simplify();
            
            // Update active tab widget
            QTabWidget* newActive = getActiveTabWidget();
            if (newActive) {
                m_activeTabWidget = newActive;
            }
            
            // Update pane numbers
            updatePaneNumbers();
        }
        
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
    
    // Reset drag state
    m_dragSourcePane = nullptr;
    m_dragSourceIndex = -1;
}

void CodeTabPane::saveActiveFile()
{
    QTabWidget* active = getActiveTabWidget();
    if (!active) return;
    
    int currentIdx = active->currentIndex();
    if (currentIdx < 0) return;
    
    CodeEditor* editor = qobject_cast<CodeEditor*>(active->widget(currentIdx));
    if (editor) {
        editor->saveFile();
        
        // Reset the modified state so future changes will trigger the dirty indicator
        editor->setModified(false);
        
        // Remove asterisk from tab name
        QString tabText = active->tabText(currentIdx);
        if (tabText.endsWith(" *")) {
            active->setTabText(currentIdx, tabText.left(tabText.length() - 2));
        }
    }
}

void CodeTabPane::saveAllActiveFiles()
{
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(allTabWidgets);
    }
    
    for (QTabWidget* tw : allTabWidgets) {
        for (int i = 0; i < tw->count(); ++i) {
            CodeEditor* editor = qobject_cast<CodeEditor*>(tw->widget(i));
            if (editor) {
                editor->saveFile();
                
                // Reset the modified state so future changes will trigger the dirty indicator
                editor->setModified(false);
                
                // Remove asterisk from tab name
                QString tabText = tw->tabText(i);
                if (tabText.endsWith(" *")) {
                    tw->setTabText(i, tabText.left(tabText.length() - 2));
                }
            }
        }
    }
}

bool CodeTabPane::fileIsOpen(const QString& path) const
{
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(const_cast<QList<QTabWidget*>&>(allTabWidgets));
    }
    
    for (QTabWidget* tw : allTabWidgets) {
        for (int i = 0; i < tw->count(); ++i) {
            CodeEditor* editor = qobject_cast<CodeEditor*>(tw->widget(i));
            if (editor && editor->getFilePath() == path) {
                return true;
            }
        }
    }
    return false;
}

void CodeTabPane::updateAllEditorsTheme(bool isDarkTheme)
{
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(allTabWidgets);
    }
    
    for (QTabWidget* tw : allTabWidgets) {
        for (int i = 0; i < tw->count(); ++i) {
            CodeEditor* editor = qobject_cast<CodeEditor*>(tw->widget(i));
            if (editor) {
                editor->updateTheme(isDarkTheme);
            }
        }
    }
}

void CodeTabPane::updateAllEditorsSettings(openide::AppSettings* settings)
{
    QList<QTabWidget*> allTabWidgets;
    if (m_root) {
        m_root->getAllTabWidgets(allTabWidgets);
    }
    
    for (QTabWidget* tw : allTabWidgets) {
        for (int i = 0; i < tw->count(); ++i) {
            CodeEditor* editor = qobject_cast<CodeEditor*>(tw->widget(i));
            if (editor) {
                editor->applySettings(settings);
            }
        }
    }
}

void CodeTabPane::updateAllSplitterStyles(bool isDarkTheme)
{
    if (!m_root) return;
    
    QString handleStyle;
    if (isDarkTheme) {
        // Dark theme: lighter handle to distinguish from dark background
        handleStyle = "QSplitter::handle { background-color: #4a4a4a; }";
    } else {
        // Light theme: slightly darker handle to distinguish from light background
        handleStyle = "QSplitter::handle { background-color: #d0d0d0; }";
    }
    
    // Recursively traverse the tree to update all splitters
    std::function<void(PaneContainer*)> updateSplitters = [&](PaneContainer* container) {
        if (!container) return;
        if (container->isBranch()) {
            QSplitter* splitter = container->splitter();
            if (splitter) {
                splitter->setStyleSheet(handleStyle);
            }
            // Recursively update children
            if (container->leftChild()) {
                updateSplitters(container->leftChild());
            }
            if (container->rightChild()) {
                updateSplitters(container->rightChild());
            }
        }
    };
    
    updateSplitters(m_root);
}
