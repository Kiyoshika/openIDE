#include "code/CodeEditor.hpp"
#include "MainWindow.hpp"
#include <QPainter>
#include <QTextBlock>
#include <QResizeEvent>
#include <QScrollBar>
#include <QFontDatabase>
#include <QApplication>

using namespace openide::code;

CodeEditor::CodeEditor(MainWindow* parent)
    : QPlainTextEdit(parent ? parent->getCentralWidget() : parent)
    , m_syntaxHighlighter{this->document()}
    , m_isModified{false}
    , m_lineNumberArea{nullptr}
    , m_isDarkTheme{false}
{
    if (!parent) return;

    // TODO: add a settings menu to change these, but these are defaults

    // font
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(11);
    setFont(font);

    // tab space
    // TODO: ideally this would be inferred from the file or fallback to a default setting
    setTabStopDistance(QFontMetricsF(font).horizontalAdvance(' ') * 4);

    // behavior
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setWordWrapMode(QTextOption::NoWrap);

    // set grid position
    QGridLayout* layout = parent->getLayout();
    layout->addWidget(this, 0, 1);
    this->setVisible(false);

    // default callback (no-op) to prevent crashes
    m_dirtyTabCallback = []{};

    // handler for dirty state (modified since last save)
    connect(this->document(), &QTextDocument::modificationChanged, this, [this](bool modified){
        m_isModified = modified;
        if (modified) m_dirtyTabCallback();
    });

    // Line number area setup
    m_lineNumberArea = new LineNumberArea(this);
    
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
    
    updateLineNumberAreaWidth(0);
    
    // Detect initial theme from system
    QPalette palette = QApplication::palette();
    QColor bgColor = palette.color(QPalette::Window);
    int luminance = (299 * bgColor.red() + 587 * bgColor.green() + 114 * bgColor.blue()) / 1000;
    m_isDarkTheme = (luminance < 128);
    
    highlightCurrentLine();
}

void CodeEditor::setModified(bool isModified)
{
    QPlainTextEdit::document()->setModified(isModified);
    m_isModified = isModified;
}

bool CodeEditor::isModified() const
{
    return m_isModified;
}

void CodeEditor::setComponentVisible(bool isVisible)
{
    this->setVisible(isVisible);
}

void CodeEditor::attachDirtyTabCallback(std::function<void()> callback)
{
    m_dirtyTabCallback = std::move(callback);
}

void CodeEditor::loadFile(const QString& path, enum FileType fileType)
{
    QFile file(path);
    if (!file.exists()) return;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream inFile(&file);
    QString fileContent = inFile.readAll();

    QStringList keywords = KeywordDictionary::getKeywordsForFileType(fileType);

    m_syntaxHighlighter.setKeywords(keywords);
    this->setPlainText(fileContent);
    m_syntaxHighlighter.rehighlight();

    m_filePath = path;
}

void CodeEditor::saveFile() const
{
    if (m_filePath.isEmpty()) return;
    QString fileContent = QPlainTextEdit::document()->toPlainText();
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << fileContent;
}

const QString& CodeEditor::getFilePath() const
{
    return m_filePath;
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    
    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);
    
    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        
        // Use theme-appropriate subtle highlight color
        QColor lineColor = m_isDarkTheme 
            ? QColor(255, 255, 255, 20)  // Subtle white for dark theme
            : QColor(0, 0, 0, 10);       // Subtle black for light theme
        
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    
    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(m_lineNumberArea);
    // Use theme-appropriate background
    QColor bgColor = m_isDarkTheme 
        ? QColor(45, 45, 45)    // Dark gray for dark theme
        : QColor(240, 240, 240); // Light gray for light theme
    painter.fillRect(event->rect(), bgColor);
    
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    
    QFont font = this->font();
    painter.setFont(font);
    
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            // Use theme-appropriate text color
            QColor textColor = m_isDarkTheme 
                ? QColor(170, 170, 170)  // Light gray for dark theme
                : QColor(100, 100, 100); // Dark gray for light theme
            painter.setPen(textColor);
            painter.drawText(0, top, m_lineNumberArea->width() - 3, fontMetrics().height(),
                           Qt::AlignRight, number);
        }
        
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::updateTheme(bool isDarkTheme)
{
    m_isDarkTheme = isDarkTheme;
    highlightCurrentLine();
    if (m_lineNumberArea) {
        m_lineNumberArea->update();
    }
}
