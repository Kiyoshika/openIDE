#include "menu/TerminalMenu.hpp"
#include "terminal/TerminalFrontend.hpp"
#include "MainWindow.hpp"

using namespace openide;
using namespace openide::menu;
using namespace openide::terminal;

TerminalMenu::TerminalMenu(MainWindow* parent, QMenuBar* menuBar) : m_parent(parent)
{
	if (!parent) return;
    
    QMenu* terminalMenu = menuBar->addMenu("&Terminal");
    
    QAction* newTerminalAction = new  QAction("New Terminal", this);
    terminalMenu->addAction(newTerminalAction);
    
    connect(newTerminalAction, &QAction::triggered, this, &TerminalMenu::onNewTerminalTriggered);
}

void TerminalMenu::onNewTerminalTriggered()
{
    if (!m_parent) return;
    TerminalFrontend& terminalFrontend = m_parent->getTerminalFrontend();
    
    // Force open the terminal - this will expand if collapsed and ensure it's visible
    terminalFrontend.forceOpen();
}
