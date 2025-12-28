#ifndef CODETABPANE_HPP
#define CODETABPANE_HPP

#include "CodeEditor.hpp"

#include <QString>
#include <QTabWidget>

// forward decl
class MainWindow;

namespace openide::code {
class CodeTabPane : QTabWidget {
public:
  CodeTabPane(MainWindow *parent);
  void addTab(CodeEditor *editor, const QString &tabName);
  void setComponentVisible(bool isVisible);
  ~CodeTabPane() = default;

private:
};
} // namespace openide::code
#endif // CODETABPANE_HPP
