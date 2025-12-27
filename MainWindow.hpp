#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "FileMenu.hpp"

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
    void initLayout();
    void initFileMenu(QMenuBar* menuBar);
    void initProjectTree();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QWidget* m_centralWidget;
    QGridLayout* m_layout;
    QTreeView* m_projectTree;
};

#endif // MAINWINDOW_HPP
