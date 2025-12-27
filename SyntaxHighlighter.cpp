#include "SyntaxHighlighter.hpp"

using namespace openide;

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent), m_keywords{{}}
{
    // TODO: add support for theme/styles at some point
    m_keywordFormat.setForeground(Qt::blue);
    m_keywordFormat.setFontWeight(QFont::Bold);
}

void SyntaxHighlighter::setKeywords(const QStringList& keywords)
{
    m_keywords = keywords;
}

void SyntaxHighlighter::rehighlight()
{
    QSyntaxHighlighter::rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    if (m_keywords.length() == 0) return;

    // TODO: test performance of regex on large files;
    // otherwise use a different tokenization method
    for (const QString& word : m_keywords)
    {
        QRegularExpression re("\\b" + QRegularExpression::escape(word) + "\\b");
        auto it = re.globalMatch(text);
        while (it.hasNext())
        {
            auto match = it.next();
            this->setFormat(match.capturedStart(), match.capturedLength(), m_keywordFormat);
        }
    }
}

SyntaxHighlighter::~SyntaxHighlighter() {}

