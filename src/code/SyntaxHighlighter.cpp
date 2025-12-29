#include "code/SyntaxHighlighter.hpp"

using namespace openide;
using namespace openide::code;

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent) 
    : QSyntaxHighlighter(parent)
    , m_treeWrapper(new TreeSitterWrapper())
    , m_colorScheme(new ColorScheme())
    , m_fileType(FileType::UNKNOWN)
    , m_isDarkTheme(false)
{
}

SyntaxHighlighter::~SyntaxHighlighter()
{
    delete m_treeWrapper;
    delete m_colorScheme;
}

void SyntaxHighlighter::setFileType(FileType fileType)
{
    m_fileType = fileType;
    m_cachedHighlights.clear();
    m_lastParsedText.clear();
}

void SyntaxHighlighter::updateTheme(bool isDarkTheme)
{
    m_isDarkTheme = isDarkTheme;
    m_colorScheme->updateTheme(isDarkTheme);
}

void SyntaxHighlighter::rehighlight()
{
    // Clear cache to force re-parsing
    m_cachedHighlights.clear();
    m_lastParsedText.clear();
    QSyntaxHighlighter::rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    if (m_fileType == FileType::UNKNOWN || !m_treeWrapper->isLanguageSupported(m_fileType))
    {
        return;
    }
    
    // Get the full document text
    QString documentText = document()->toPlainText();
    
    // Only re-parse if the document has changed
    if (documentText != m_lastParsedText)
    {
        m_lastParsedText = documentText;
        m_cachedHighlights = m_treeWrapper->parseAndHighlight(documentText, m_fileType);
    }
    
    // Get the position of the current block in the document
    int blockStart = currentBlock().position();
    int blockLength = currentBlock().length();
    
    // Apply highlights for this block
    for (const HighlightInfo& info : m_cachedHighlights)
    {
        // Check if this highlight overlaps with the current block
        int highlightStart = info.start;
        int highlightEnd = info.start + info.length;
        int blockEnd = blockStart + blockLength;
        
        // Skip highlights that don't overlap with this block
        if (highlightEnd <= blockStart || highlightStart >= blockEnd)
        {
            continue;
        }
        
        // Calculate the position and length within the current block
        int formatStart = qMax(0, highlightStart - blockStart);
        int formatEnd = qMin(blockLength, highlightEnd - blockStart);
        int formatLength = formatEnd - formatStart;
        
        if (formatLength > 0)
        {
            QTextCharFormat format = m_colorScheme->getFormatForType(info.type);
            setFormat(formatStart, formatLength, format);
        }
    }
}

