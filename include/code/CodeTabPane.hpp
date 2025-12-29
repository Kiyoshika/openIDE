#ifndef CODETABPANE_HPP
#define CODETABPANE_HPP

#include "CodeEditor.hpp"
#include "PaneContainer.hpp"

#include <QString>
#include <QWidget>
#include <QTabWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QMenu>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDrag>
#include <QTabBar>

// forward decl
class MainWindow;
namespace openide { class AppSettings; }

namespace openide::code {
class CodeTabPane : public QWidget {
  Q_OBJECT
public:
  CodeTabPane(MainWindow *parent);
  void addTab(CodeEditor *editor, const QString &tabName);
  void setComponentVisible(bool isVisible);
  void saveActiveFile();
  void saveAllActiveFiles();
  bool fileIsOpen(const QString& path) const;
  void updateAllEditorsTheme(bool isDarkTheme);
  void updateAllEditorsSettings(openide::AppSettings* settings);
  
  // For compatibility with external code expecting QTabWidget interface
  int count() const;
  QWidget* widget(int index) const;
  int currentIndex() const;
  QWidget* currentWidget() const;
  void setTabText(int index, const QString& text);
  
  ~CodeTabPane() = default;

signals:
  void editMenuStateChanged();

protected:
  bool eventFilter(QObject* obj, QEvent* event) override;
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

private:
  void showTabContextMenu(const QPoint& pos, QTabWidget* pane, int tabIndex);
  void closeTab(QTabWidget* pane, int tabIndex);
  void closeAllOtherTabs(QTabWidget* pane, int tabIndex);
  void closeAllTabsInPane(QTabWidget* pane);
  void closeAllTabs();
  void splitHorizontal(QTabWidget* pane, int tabIndex);
  void splitVertical(QTabWidget* pane, int tabIndex);
  void moveTabToPane(QTabWidget* sourcePane, int tabIndex, QTabWidget* targetPane);
  void setupTabWidget(QTabWidget* tabWidget);
  void connectEditorSignals(CodeEditor* editor, QTabWidget* tabWidget);
  CodeEditor* duplicateEditor(CodeEditor* source);
  PaneContainer* findContainerForTabWidget(QTabWidget* tabWidget);
  QTabWidget* getActiveTabWidget();
  void updatePaneNumbers();
  
  void saveFileByIndex_(int index);
  void dirtyTabByIndex_(int index);
  void undirtyTabByIndex_(int index);
  
  MainWindow* m_parent;
  QVBoxLayout* m_layout;
  PaneContainer* m_root;
  QTabWidget* m_activeTabWidget;
  int m_contextMenuTabIndex;
  QTabWidget* m_contextMenuPane;
  
  // For drag and drop between panes
  QTabWidget* m_dragSourcePane;
  int m_dragSourceIndex;
};
} // namespace openide::code
#endif // CODETABPANE_HPP
