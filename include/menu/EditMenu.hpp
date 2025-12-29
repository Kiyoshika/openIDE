#ifndef EDITMENU_HPP
#define EDITMENU_HPP

#include <QMenu>
#include <QMenuBar>
#include <QAction>

// forward decl
class MainWindow;
namespace openide::code { class CodeTabPane; }

namespace openide::menu
{
class EditMenu : public QMenu
{
    Q_OBJECT
public:
    EditMenu(MainWindow* parent, QMenuBar* menuBar, openide::code::CodeTabPane* codeTabPane);
    ~EditMenu() = default;
    
    void updateFindActionState();
    
private slots:
    void onFindTriggered();
    
private:
    MainWindow* m_mainWindow;
    openide::code::CodeTabPane* m_codeTabPane;
    QAction* m_findAction;
};
}

#endif // EDITMENU_HPP


