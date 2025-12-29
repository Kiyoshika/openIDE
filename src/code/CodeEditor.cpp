#include "code/CodeEditor.hpp"
#include "code/FindReplaceDialog.hpp"
#include "MainWindow.hpp"
#include "AppSettings.hpp"
#include <QPainter>
#include <QTextBlock>
#include <QResizeEvent>
#include <QScrollBar>
#include <QFontDatabase>
#include <QApplication>
#include <QShortcut>
#include <QKeySequence>

using namespace openide::code;

CodeEditor::CodeEditor(MainWindow* parent, openide::AppSettings* settings)
    : QPlainTextEdit(parent ? parent->getCentralWidget() : parent)
    , m_parent{parent}
    , m_syntaxHighlighter{this->document()}
    , m_isModified{false}
    , m_lineNumberArea{nullptr}
    , m_isDarkTheme{false}
    , m_findReplaceDialog{nullptr}
{
    if (!parent) return;

    // Apply settings if provided, otherwise use defaults
    if (settings) {
        applySettings(settings);
    } else {
        // Fallback to defaults if no settings provided
        QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        font.setPointSize(12);
        setFont(font);
        setTabStopDistance(QFontMetricsF(font).horizontalAdvance(' ') * 4);
    }

    // behavior
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setWordWrapMode(QTextOption::NoWrap);

    // set grid position
    QGridLayout* layout = parent->getLayout();
    layout->addWidget(this, 0, 1);
    this->setVisible(false);

    // handler for dirty state (modified since last save)
    connect(this->document(), &QTextDocument::modificationChanged, this, [this](bool modified){
        m_isModified = modified;
        if (modified) {
            emit fileModified();
        }
    });

    // Line number area setup
    m_lineNumberArea = new LineNumberArea(this);
    m_lineNumberArea->setAttribute(Qt::WA_OpaquePaintEvent);
    m_lineNumberArea->setAutoFillBackground(false);
    
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
    
    updateLineNumberAreaWidth(0);
    
    // Detect initial theme from system
    QPalette palette = QApplication::palette();
    QColor bgColor = palette.color(QPalette::Window);
    int luminance = (299 * bgColor.red() + 587 * bgColor.green() + 114 * bgColor.blue()) / 1000;
    m_isDarkTheme = (luminance < 128);
    
    // Initialize syntax highlighter with detected theme
    m_syntaxHighlighter.updateTheme(m_isDarkTheme);
    
    highlightCurrentLine();
    
    // Add Ctrl+F shortcut for find and replace
    QShortcut* findShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this);
    connect(findShortcut, &QShortcut::activated, this, &CodeEditor::showFindReplaceDialog);
}

CodeEditor::~CodeEditor()
{
    if (m_findReplaceDialog) {
        delete m_findReplaceDialog;
        m_findReplaceDialog = nullptr;
    }
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


void CodeEditor::loadFile(const QString& path, enum FileType fileType)
{
    QFile file(path);
    if (!file.exists()) return;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream inFile(&file);
    QString fileContent = inFile.readAll();

    // Set file type for tree-sitter syntax highlighting
    m_syntaxHighlighter.setFileType(fileType);
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

void CodeEditor::keyPressEvent(QKeyEvent* event)
{
    QTextCursor cursor = textCursor();
    
    // Handle Tab key with selection - indent all selected lines
    if (event->key() == Qt::Key_Tab && cursor.hasSelection()) {
        int selectionStart = cursor.selectionStart();
        int selectionEnd = cursor.selectionEnd();
        
        // Find the blocks (lines) that are selected
        QTextBlock startBlock = document()->findBlock(selectionStart);
        QTextBlock endBlock = document()->findBlock(selectionEnd);
        
        // Calculate indent string based on tab stop distance
        QFontMetricsF metrics(font());
        int spacesPerTab = static_cast<int>(tabStopDistance() / metrics.horizontalAdvance(' '));
        QString indent = QString(spacesPerTab, ' ');
        
        // Begin edit block for undo/redo
        cursor.beginEditBlock();
        
        // Iterate through all selected lines and indent them
        QTextBlock block = startBlock;
        while (block.isValid()) {
            cursor.setPosition(block.position());
            cursor.insertText(indent);
            
            if (block == endBlock)
                break;
            block = block.next();
        }
        
        cursor.endEditBlock();
        
        // Restore selection with adjusted positions
        cursor.setPosition(startBlock.position());
        cursor.setPosition(endBlock.position() + endBlock.length() - 1, QTextCursor::KeepAnchor);
        setTextCursor(cursor);
        
        event->accept();
        return;
    }
    
    // Handle Shift+Tab with selection - unindent all selected lines
    if (event->key() == Qt::Key_Backtab && cursor.hasSelection()) {
        int selectionStart = cursor.selectionStart();
        int selectionEnd = cursor.selectionEnd();
        
        // Find the blocks (lines) that are selected
        QTextBlock startBlock = document()->findBlock(selectionStart);
        QTextBlock endBlock = document()->findBlock(selectionEnd);
        
        // Calculate spaces per tab
        QFontMetricsF metrics(font());
        int spacesPerTab = static_cast<int>(tabStopDistance() / metrics.horizontalAdvance(' '));
        
        // Begin edit block for undo/redo
        cursor.beginEditBlock();
        
        // Iterate through all selected lines and unindent them
        QTextBlock block = startBlock;
        while (block.isValid()) {
            cursor.setPosition(block.position());
            QString text = block.text();
            
            // Remove up to spacesPerTab spaces from the beginning
            int spacesToRemove = 0;
            for (int i = 0; i < spacesPerTab && i < text.length(); ++i) {
                if (text[i] == ' ') {
                    spacesToRemove++;
                } else {
                    break;
                }
            }
            
            if (spacesToRemove > 0) {
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, spacesToRemove);
                cursor.removeSelectedText();
            }
            
            if (block == endBlock)
                break;
            block = block.next();
        }
        
        cursor.endEditBlock();
        
        // Restore selection with adjusted positions
        cursor.setPosition(startBlock.position());
        cursor.setPosition(endBlock.position() + endBlock.length() - 1, QTextCursor::KeepAnchor);
        setTextCursor(cursor);
        
        event->accept();
        return;
    }
    
    // Handle Enter key - smart auto-indentation
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // Get current line text
        QTextBlock currentBlock = cursor.block();
        QString currentLineText = currentBlock.text();
        
        // Calculate leading whitespace
        int indentCount = 0;
        for (const QChar& ch : currentLineText) {
            if (ch == ' ') {
                indentCount++;
            } else if (ch == '\t') {
                // Convert tabs to spaces based on tab stop distance
                QFontMetricsF metrics(font());
                int spacesPerTab = static_cast<int>(tabStopDistance() / metrics.horizontalAdvance(' '));
                indentCount += spacesPerTab;
            } else {
                break;
            }
        }
        
        // Insert newline followed by the same indentation
        QString indent = QString(indentCount, ' ');
        cursor.insertText("\n" + indent);
        
        event->accept();
        return;
    }
    
    // For all other keys, use default behavior
    QPlainTextEdit::keyPressEvent(event);
}

void CodeEditor::wheelEvent(QWheelEvent* event)
{
    // Handle Ctrl+scroll for font size adjustment
    if (event->modifiers() & Qt::ControlModifier) {
        // Get the scroll direction
        int delta = event->angleDelta().y();
        
        if (delta != 0) {
            // Get current font and size
            QFont currentFont = font();
            int currentSize = currentFont.pointSize();
            
            // Adjust size based on scroll direction
            // Clamp between reasonable bounds (6-72)
            int newSize = currentSize;
            if (delta > 0) {
                newSize = qMin(72, currentSize + 1);  // Scroll up = increase
            } else {
                newSize = qMax(6, currentSize - 1);   // Scroll down = decrease
            }
            
            // Only update if size actually changed
            if (newSize != currentSize) {
                // Save the new font size to settings and persist to file
                openide::AppSettings* settings = m_parent->getAppSettings();
                settings->setFontSize(newSize);
                settings->saveToFile();
                
                // Update all open editors with the new settings
                m_parent->getCodeTabPane()->updateAllEditorsSettings(settings);
            }
            
            event->accept();
            return;
        }
    }
    
    // For non-Ctrl scroll or no vertical scroll, use default behavior
    QPlainTextEdit::wheelEvent(event);
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
    // Fill the entire widget area, not just the event rect, to ensure complete background update
    painter.fillRect(m_lineNumberArea->rect(), bgColor);
    
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
    
    // Update syntax highlighter theme
    m_syntaxHighlighter.updateTheme(isDarkTheme);
    m_syntaxHighlighter.rehighlight();
    
    highlightCurrentLine();
    if (m_lineNumberArea) {
        // Force complete repaint of the line number area
        m_lineNumberArea->setAttribute(Qt::WA_OpaquePaintEvent);
        m_lineNumberArea->update();
        m_lineNumberArea->repaint();
    }
    // Also update the viewport
    viewport()->update();
}

void CodeEditor::applySettings(openide::AppSettings* settings)
{
    if (!settings) return;
    
    QFont font = settings->font();
    setFont(font);
    setTabStopDistance(QFontMetricsF(font).horizontalAdvance(' ') * settings->tabSpace());
    
    // Update line number area if it exists
    if (m_lineNumberArea) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::showFindReplaceDialog()
{
    if (!m_findReplaceDialog) {
        m_findReplaceDialog = new FindReplaceDialog(this, m_parent);
    }
    m_findReplaceDialog->show();
    m_findReplaceDialog->raise();
    m_findReplaceDialog->activateWindow();
}
