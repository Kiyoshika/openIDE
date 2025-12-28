#include "code/CodeEditor.hpp"
#include "MainWindow.hpp"

using namespace openide::code;

CodeEditor::CodeEditor(MainWindow* parent)
    : QPlainTextEdit(parent ? parent->getCentralWidget() : parent)
    , m_syntaxHighlighter{this->document()}
    , m_isModified{false}
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
