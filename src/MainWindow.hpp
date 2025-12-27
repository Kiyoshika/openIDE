#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "FileMenu.hpp"
#include "ProjectTree.hpp"
#include "CodeEditor.hpp"
#include "CodeTabPane.hpp"

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QObject>
#include <QTreeView>
#include <QLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QWidget* getCentralWidget() const;
    QGridLayout* getLayout() const;
    void initLayout();
    void initFileMenu(QMenuBar* menuBar);
    void initProjectTree();
    void setComponentsVisible(bool isVisible);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QWidget* m_centralWidget;
    QGridLayout* m_layout;
    openide::ProjectTree m_projectTree;
    openide::code::CodeTabPane m_codeTabPane;
};

#endif // MAINWINDOW_HPP
