#include "CodeEditor.hpp"
#include "MainWindow.hpp"

using namespace openide;

CodeEditor::CodeEditor(MainWindow* parent) : QPlainTextEdit(parent ? parent->getCentralWidget() : parent), m_syntaxHighlighter{this->document()}
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

    QStringList keywords = KeywordDictionary::getKeywordsForFileType(fileType);

    m_syntaxHighlighter.setKeywords(keywords);
    this->setPlainText(fileContent);
    m_syntaxHighlighter.rehighlight();
}

CodeEditor::~CodeEditor() {}
