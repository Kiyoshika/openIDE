#ifndef COLORSCHEME_HPP
#define COLORSCHEME_HPP

#include <QTextCharFormat>
#include <QColor>
#include <QFont>
#include <QMap>

namespace openide::code
{
enum class HighlightType
{
    Keyword,
    Comment,
    String,
    Number,
    Function,
    Class,
    Type,
    Variable,
    Operator,
    Punctuation,
    Property,
    Parameter,
    Constant,
    None
};

class ColorScheme
{
public:
    ColorScheme();
    void updateTheme(bool isDarkTheme);
    QTextCharFormat getFormatForType(HighlightType type) const;
    bool isDarkTheme() const;

private:
    void initializeLightTheme();
    void initializeDarkTheme();
    
    bool m_isDarkTheme;
    QMap<HighlightType, QTextCharFormat> m_formats;
};
}
#endif // COLORSCHEME_HPP

