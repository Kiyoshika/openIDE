#include "menu/EditMenu.hpp"
#include "MainWindow.hpp"
#include "code/CodeTabPane.hpp"
#include "code/CodeEditor.hpp"

using namespace openide::menu;

EditMenu::EditMenu(MainWindow* parent, QMenuBar* menuBar, openide::code::CodeTabPane* codeTabPane)
    : QMenu(parent)
    , m_mainWindow(parent)
    , m_codeTabPane(codeTabPane)
    , m_findAction(nullptr)
{
    if (!parent || !menuBar) return;

    QMenu* editMenu = menuBar->addMenu("&Edit");
    
    // Find action (shortcut is handled by CodeEditor)
    m_findAction = new QAction("&Find\tCtrl+F", this);
    editMenu->addAction(m_findAction);
    
    // Initially disabled until a CodeEditor is active
    m_findAction->setEnabled(false);
    
    // Connect actions
    connect(m_findAction, &QAction::triggered, this, &EditMenu::onFindTriggered);
}

void EditMenu::updateFindActionState()
{
    // Enable Find action only if there's an active CodeEditor
    bool hasActiveEditor = m_codeTabPane && m_codeTabPane->count() > 0;
    if (m_findAction) {
        m_findAction->setEnabled(hasActiveEditor);
    }
}

void EditMenu::onFindTriggered()
{
    if (!m_codeTabPane) return;
    
    int currentIndex = m_codeTabPane->currentIndex();
    if (currentIndex < 0 || currentIndex >= m_codeTabPane->count()) return;
    
    // Get the current editor
    QWidget* widget = m_codeTabPane->widget(currentIndex);
    openide::code::CodeEditor* editor = qobject_cast<openide::code::CodeEditor*>(widget);
    
    if (editor) {
        editor->showFindReplaceDialog();
    }
}

