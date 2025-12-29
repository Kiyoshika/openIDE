#include "code/ColorScheme.hpp"

using namespace openide::code;

ColorScheme::ColorScheme()
    : m_isDarkTheme(false)
{
    initializeLightTheme();
}

void ColorScheme::updateTheme(bool isDarkTheme)
{
    if (m_isDarkTheme != isDarkTheme)
    {
        m_isDarkTheme = isDarkTheme;
        if (isDarkTheme)
        {
            initializeDarkTheme();
        }
        else
        {
            initializeLightTheme();
        }
    }
}

QTextCharFormat ColorScheme::getFormatForType(HighlightType type) const
{
    return m_formats.value(type, QTextCharFormat());
}

bool ColorScheme::isDarkTheme() const
{
    return m_isDarkTheme;
}

void ColorScheme::initializeLightTheme()
{
    m_formats.clear();
    
    // Keywords - blue, bold
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(0x00, 0x00, 0xFF)); // #0000FF
    keywordFormat.setFontWeight(QFont::Bold);
    m_formats[HighlightType::Keyword] = keywordFormat;
    
    // Comments - green, italic
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(0x00, 0x80, 0x00)); // #008000
    commentFormat.setFontItalic(true);
    m_formats[HighlightType::Comment] = commentFormat;
    
    // Strings - red
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(0xA3, 0x15, 0x15)); // #A31515
    m_formats[HighlightType::String] = stringFormat;
    
    // Numbers - teal
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(0x09, 0x86, 0x58)); // #098658
    m_formats[HighlightType::Number] = numberFormat;
    
    // Functions - brown
    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor(0x79, 0x5E, 0x26)); // #795E26
    m_formats[HighlightType::Function] = functionFormat;
    
    // Classes/Types - cyan
    QTextCharFormat classFormat;
    classFormat.setForeground(QColor(0x26, 0x7F, 0x99)); // #267F99
    classFormat.setFontWeight(QFont::Bold);
    m_formats[HighlightType::Class] = classFormat;
    m_formats[HighlightType::Type] = classFormat;
    
    // Variables - dark blue
    QTextCharFormat variableFormat;
    variableFormat.setForeground(QColor(0x00, 0x10, 0x80)); // #001080
    m_formats[HighlightType::Variable] = variableFormat;
    
    // Properties - similar to variables
    QTextCharFormat propertyFormat;
    propertyFormat.setForeground(QColor(0x00, 0x10, 0x80)); // #001080
    m_formats[HighlightType::Property] = propertyFormat;
    
    // Parameters - similar to variables
    QTextCharFormat parameterFormat;
    parameterFormat.setForeground(QColor(0x00, 0x10, 0x80)); // #001080
    m_formats[HighlightType::Parameter] = parameterFormat;
    
    // Constants - similar to numbers
    QTextCharFormat constantFormat;
    constantFormat.setForeground(QColor(0x09, 0x86, 0x58)); // #098658
    constantFormat.setFontWeight(QFont::Bold);
    m_formats[HighlightType::Constant] = constantFormat;
    
    // Operators - black
    QTextCharFormat operatorFormat;
    operatorFormat.setForeground(QColor(0x00, 0x00, 0x00)); // #000000
    m_formats[HighlightType::Operator] = operatorFormat;
    
    // Punctuation - black
    QTextCharFormat punctuationFormat;
    punctuationFormat.setForeground(QColor(0x00, 0x00, 0x00)); // #000000
    m_formats[HighlightType::Punctuation] = punctuationFormat;
}

void ColorScheme::initializeDarkTheme()
{
    m_formats.clear();
    
    // Keywords - light blue, bold
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(0x56, 0x9C, 0xD6)); // #569CD6
    keywordFormat.setFontWeight(QFont::Bold);
    m_formats[HighlightType::Keyword] = keywordFormat;
    
    // Comments - light green, italic
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(0x6A, 0x99, 0x55)); // #6A9955
    commentFormat.setFontItalic(true);
    m_formats[HighlightType::Comment] = commentFormat;
    
    // Strings - light red/orange
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(0xCE, 0x91, 0x78)); // #CE9178
    m_formats[HighlightType::String] = stringFormat;
    
    // Numbers - light teal
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(0xB5, 0xCE, 0xA8)); // #B5CEA8
    m_formats[HighlightType::Number] = numberFormat;
    
    // Functions - yellow
    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor(0xDC, 0xDC, 0xAA)); // #DCDCAA
    m_formats[HighlightType::Function] = functionFormat;
    
    // Classes/Types - cyan/turquoise
    QTextCharFormat classFormat;
    classFormat.setForeground(QColor(0x4E, 0xC9, 0xB0)); // #4EC9B0
    classFormat.setFontWeight(QFont::Bold);
    m_formats[HighlightType::Class] = classFormat;
    m_formats[HighlightType::Type] = classFormat;
    
    // Variables - light blue
    QTextCharFormat variableFormat;
    variableFormat.setForeground(QColor(0x9C, 0xDC, 0xFE)); // #9CDCFE
    m_formats[HighlightType::Variable] = variableFormat;
    
    // Properties - light blue
    QTextCharFormat propertyFormat;
    propertyFormat.setForeground(QColor(0x9C, 0xDC, 0xFE)); // #9CDCFE
    m_formats[HighlightType::Property] = propertyFormat;
    
    // Parameters - light blue
    QTextCharFormat parameterFormat;
    parameterFormat.setForeground(QColor(0x9C, 0xDC, 0xFE)); // #9CDCFE
    m_formats[HighlightType::Parameter] = parameterFormat;
    
    // Constants - light teal, bold
    QTextCharFormat constantFormat;
    constantFormat.setForeground(QColor(0xB5, 0xCE, 0xA8)); // #B5CEA8
    constantFormat.setFontWeight(QFont::Bold);
    m_formats[HighlightType::Constant] = constantFormat;
    
    // Operators - light gray
    QTextCharFormat operatorFormat;
    operatorFormat.setForeground(QColor(0xD4, 0xD4, 0xD4)); // #D4D4D4
    m_formats[HighlightType::Operator] = operatorFormat;
    
    // Punctuation - light gray
    QTextCharFormat punctuationFormat;
    punctuationFormat.setForeground(QColor(0xD4, 0xD4, 0xD4)); // #D4D4D4
    m_formats[HighlightType::Punctuation] = punctuationFormat;
}


