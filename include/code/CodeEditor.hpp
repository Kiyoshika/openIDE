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
#include <QTabWidget>

#include <functional>

namespace openide::code
{
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    CodeEditor(MainWindow* parent = nullptr);
    void setComponentVisible(bool isVisible);
    void attachDirtyTabCallback(std::function<void()> callback);
    void loadFile(const QString& path, enum FileType fileType);
    void saveFile() const;
    void setModified(bool isModified);
    bool isModified() const;
    ~CodeEditor() = default;
private:
    bool m_isModified;
    QString m_filePath;
    openide::SyntaxHighlighter m_syntaxHighlighter;
    std::function<void()> m_dirtyTabCallback;
};
}
#endif // CODEEDITOR_HPP
