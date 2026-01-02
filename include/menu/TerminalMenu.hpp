#ifndef TERMINALMENU_HPP
#define TERMINALMENU_HPP

#include <QWidget>
#include <QMenu>
#include <QMenuBar>

// forward decl
class MainWindow;

namespace openide::menu
{
class TerminalMenu : public QMenu
{
	Q_OBJECT
 public:
    TerminalMenu(MainWindow* parent, QMenuBar* menuBar);
     ~TerminalMenu() = default;
private slots:
	void onNewTerminalTriggered();
private:
};
}
#endif // TERMINALMENU_HPP
