#include "CodeTabPane.hpp"
#include "CodeEditor.hpp"
#include "MainWindow.hpp"

using namespace openide::code;

CodeTabPane::CodeTabPane(MainWindow *parent)
    : QTabWidget(parent ? parent->getCentralWidget() : parent)
{
    // styling
    QTabWidget::setTabsClosable(true);
    QTabWidget::setTabShape(TabShape::Rounded);
    QTabWidget::setVisible(false);
    QTabWidget::setMovable(true);
    QTabWidget::setDocumentMode(true);
    QTabWidget::setElideMode(Qt::ElideRight);

    // grid placement
    if (parent->getLayout())
        parent->getLayout()->addWidget(this, 0, 1);

    // handler to close the tab
    QTabWidget::connect(this, &QTabWidget::tabCloseRequested, parent, [this](int index){
        QWidget* page = QTabWidget::widget(index);
        QTabWidget::removeTab(index);
        page->deleteLater();
    });
}

void CodeTabPane::addTab(CodeEditor* editor, const QString& tabName)
{
    QTabWidget::addTab(editor, tabName);
    setCurrentWidget(editor);
}

void CodeTabPane::setComponentVisible(bool isVisible)
{
    QTabWidget::setVisible(isVisible);
}
