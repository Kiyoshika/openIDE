#include "code/CodeTabPane.hpp"
#include "code/CodeEditor.hpp"
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

    // create shortcut to save files
    QAction* saveAction = new QAction("&Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setShortcutContext(Qt::WindowShortcut);
    saveAction->setEnabled(true);

    addAction(saveAction);

    QTabWidget::tabPosition();

    // save file in current tab handler
    connect(saveAction, &QAction::triggered, this, [this] {
        CodeEditor* activeEditor = qobject_cast<CodeEditor*>(QTabWidget::currentWidget());
        if (!activeEditor) return;
        activeEditor->saveFile();
        undirtyTabByIndex_(QTabWidget::currentIndex());
        activeEditor->setModified(false);
    });
}

void CodeTabPane::addTab(CodeEditor* editor, const QString& tabName)
{
    if (!editor) return;
    // attach the dirtyTabCallback to the editor to mark when the file
    // is edited to update the tab text
    editor->attachDirtyTabCallback([this](){
        int index = QTabWidget::currentIndex();
        dirtyTabByIndex_(index);

    });
    QTabWidget::addTab(editor, tabName);
    setCurrentWidget(editor);
}

void CodeTabPane::setComponentVisible(bool isVisible)
{
    QTabWidget::setVisible(isVisible);
}

void CodeTabPane::saveActiveFile()
{
    saveFileByIndex_(QTabWidget::currentIndex());
}

void CodeTabPane::saveAllActiveFiles()
{
    for (int i = 0; i < QTabWidget::count(); ++i)
        saveFileByIndex_(i);
}

void CodeTabPane::saveFileByIndex_(int index)
{
    if (index < 0 || index >= QTabWidget::count()) return;

    CodeEditor* editor = qobject_cast<CodeEditor*>(QTabWidget::widget(index));
    if (!editor) return;

    if (editor->isModified())
    {
        editor->saveFile();
        undirtyTabByIndex_(index);
        editor->setModified(false);
    }
}

void CodeTabPane::dirtyTabByIndex_(int index)
{
    if (index < 0 || index >= QTabWidget::count()) return;

    const QString& tabName = QTabWidget::tabText(index);
    if (!tabName.endsWith(" *"))
        QTabWidget::setTabText(index, tabName + " *");
}

void CodeTabPane::undirtyTabByIndex_(int index)
{
    if (index < 0 || index >= QTabWidget::count()) return;

    const QString& tabName = QTabWidget::tabText(index);
    if (tabName.endsWith(" *"))
        QTabWidget::setTabText(index, tabName.left(tabName.length() - 2));
}
