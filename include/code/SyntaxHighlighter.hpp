#ifndef SYNTAXHIGHLIGHTER_HPP
#define SYNTAXHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

namespace openide
{
class SyntaxHighlighter : QSyntaxHighlighter
{
public:
    SyntaxHighlighter(QTextDocument* parent);
    void setKeywords(const QStringList& keywords);
    void highlightBlock(const QString& text) override;
    void rehighlight();
    ~SyntaxHighlighter();
private:
    QStringList m_keywords;
    QTextCharFormat m_keywordFormat;
};
}
#endif // SYNTAXHIGHLIGHTER_HPP
