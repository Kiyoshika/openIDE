#ifndef SYNTAXHIGHLIGHTER_HPP
#define SYNTAXHIGHLIGHTER_HPP

#include "FileType.hpp"
#include "code/TreeSitterWrapper.hpp"
#include "code/ColorScheme.hpp"
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QVector>

namespace openide
{
class SyntaxHighlighter : QSyntaxHighlighter
{
public:
    SyntaxHighlighter(QTextDocument* parent);
    void setFileType(enum FileType fileType);
    void updateTheme(bool isDarkTheme);
    void highlightBlock(const QString& text) override;
    void rehighlight();
    ~SyntaxHighlighter();
    
private:
    void applyHighlights();
    
    openide::code::TreeSitterWrapper* m_treeWrapper;
    openide::code::ColorScheme* m_colorScheme;
    FileType m_fileType;
    bool m_isDarkTheme;
    QVector<openide::code::HighlightInfo> m_cachedHighlights;
    QString m_lastParsedText;
};
}
#endif // SYNTAXHIGHLIGHTER_HPP
