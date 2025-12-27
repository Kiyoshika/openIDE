#include "SyntaxHighlighter.hpp"

using namespace openide;

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent), m_keywords{{}}
{
    // TODO: add support for theme/styles at some point
    // need to support major/minor keywords, comments, string/char/numeric literals, etc.
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

    // TODO: replace this with TreeSitter; this is just a placeholder
    // to have some level of highlighting
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

