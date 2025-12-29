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
#include <QResizeEvent>
#include <QPaintEvent>

#include <functional>

namespace openide::code
{
class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
    friend class LineNumberArea;
public:
    CodeEditor(MainWindow* parent = nullptr);
    void setComponentVisible(bool isVisible);
    void attachDirtyTabCallback(std::function<void()> callback);
    const QString& getFilePath() const;
    void loadFile(const QString& path, enum FileType fileType);
    void saveFile() const;
    void setModified(bool isModified);
    bool isModified() const;
    ~CodeEditor() = default;
protected:
    void resizeEvent(QResizeEvent* event) override;
private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect& rect, int dy);
private:
    int lineNumberAreaWidth();
    void lineNumberAreaPaintEvent(QPaintEvent* event);
    
    bool m_isModified;
    QString m_filePath;
    openide::SyntaxHighlighter m_syntaxHighlighter;
    std::function<void()> m_dirtyTabCallback;
    LineNumberArea* m_lineNumberArea;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor* editor) : QWidget(editor), m_codeEditor(editor) {}
    
    QSize sizeHint() const override
    {
        return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
    }
    
protected:
    void paintEvent(QPaintEvent* event) override
    {
        m_codeEditor->lineNumberAreaPaintEvent(event);
    }
    
private:
    CodeEditor* m_codeEditor;
};
}
#endif // CODEEDITOR_HPP
