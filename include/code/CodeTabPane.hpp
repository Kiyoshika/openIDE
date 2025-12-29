#ifndef CODETABPANE_HPP
#define CODETABPANE_HPP

#include "CodeEditor.hpp"

#include <QString>
#include <QTabWidget>

// forward decl
class MainWindow;
namespace openide { class AppSettings; }

namespace openide::code {
class CodeTabPane : QTabWidget {
public:
  CodeTabPane(MainWindow *parent);
  void addTab(CodeEditor *editor, const QString &tabName);
  void setComponentVisible(bool isVisible);
  void saveActiveFile();
  void saveAllActiveFiles();
  bool fileIsOpen(const QString& path) const;
  void updateAllEditorsTheme(bool isDarkTheme);
  void updateAllEditorsSettings(openide::AppSettings* settings);
  ~CodeTabPane() = default;

private:
    void saveFileByIndex_(int index);
    void dirtyTabByIndex_(int index);
    void undirtyTabByIndex_(int index);
};
} // namespace openide::code
#endif // CODETABPANE_HPP
