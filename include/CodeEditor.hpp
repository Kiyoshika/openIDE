#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP

// forward decl
class MainWindow;

#include "FileType.hpp"
#include "KeywordDictionary.hpp"
#include "SyntaxHighlighter.hpp"

#include <QPlainTextEdit>
#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QSyntaxHighlighter>

namespace openide
{
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    CodeEditor(MainWindow* parent = nullptr);
    void setComponentVisible(bool isVisible);
    void loadFile(const QString& path, enum FileType fileType);
    ~CodeEditor();
private:
    openide::SyntaxHighlighter m_syntaxHighlighter;
};
}
#endif // CODEEDITOR_HPP
