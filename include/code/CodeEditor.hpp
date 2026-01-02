#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP

// forward decl
class MainWindow;
namespace openide { class AppSettings; }
namespace openide::code { class FindReplaceDialog; }

#include "FileType.hpp"
#include "SyntaxHighlighter.hpp"

#include <QPlainTextEdit>
#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QSyntaxHighlighter>
#include <QTabWidget>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QWheelEvent>

namespace openide::code
{
class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
    friend class LineNumberArea;
public:
    CodeEditor(MainWindow* parent = nullptr, openide::AppSettings* settings = nullptr);
    void setComponentVisible(bool isVisible);
    void applySettings(openide::AppSettings* settings);
    const QString& getFilePath() const;
    void loadFile(const QString& path, enum FileType fileType);
    void saveFile() const;
    void setModified(bool isModified);
    bool isModified() const;
    void updateTheme(bool isDarkTheme);
    void showFindReplaceDialog();
    ~CodeEditor();
    
signals:
    void fileModified();
protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect& rect, int dy);
private:
    int lineNumberAreaWidth();
    void lineNumberAreaPaintEvent(QPaintEvent* event);
    
    MainWindow* m_parent;
    bool m_isDarkTheme;
    bool m_isModified;
    QString m_filePath;
    openide::SyntaxHighlighter m_syntaxHighlighter;
    LineNumberArea* m_lineNumberArea;
    FindReplaceDialog* m_findReplaceDialog;
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
