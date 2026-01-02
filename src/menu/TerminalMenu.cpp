#include "menu/TerminalMenu.hpp"
#include "MainWindow.hpp"

using namespace openide;
using namespace openide::menu;

TerminalMenu::TerminalMenu(MainWindow* parent, QMenuBar* menuBar)
{
	if (!parent) return;
    
    QMenu* terminalMenu = menuBar->addMenu("&Terminal");
    
    QAction* newTerminalAction = new  QAction("New Terminal", this);
    terminalMenu->addAction(newTerminalAction);
    
    connect(newTerminalAction, &QAction::triggered, this, &TerminalMenu::onNewTerminalTriggered);
}

void TerminalMenu::onNewTerminalTriggered()
{
	// TODO: implement this
}