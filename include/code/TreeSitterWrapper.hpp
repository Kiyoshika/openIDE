#ifndef TREESITTERWRAPPER_HPP
#define TREESITTERWRAPPER_HPP

#include "FileType.hpp"
#include "code/ColorScheme.hpp"
#include <QString>
#include <QVector>
#include <QMap>

// Forward declarations for tree-sitter types
typedef struct TSParser TSParser;
typedef struct TSTree TSTree;
typedef struct TSLanguage TSLanguage;
typedef struct TSQuery TSQuery;
typedef struct TSQueryCursor TSQueryCursor;

namespace openide::code
{
struct HighlightInfo
{
    int start;
    int length;
    HighlightType type;
};

class TreeSitterWrapper
{
public:
    TreeSitterWrapper();
    ~TreeSitterWrapper();
    
    // Parse text and extract highlights
    QVector<HighlightInfo> parseAndHighlight(const QString& text, openide::FileType fileType);
    
    // Check if a language is supported
    bool isLanguageSupported(openide::FileType fileType) const;
    
private:
    // Get tree-sitter language for file type
    const TSLanguage* getLanguage(openide::FileType fileType) const;
    
    // Get query patterns for file type
    const char* getQueryPatterns(openide::FileType fileType) const;
    
    // Extract highlights from parse tree using queries
    QVector<HighlightInfo> extractHighlights(const QString& text, TSTree* tree, openide::FileType fileType);
    
    // Map capture names to highlight types
    HighlightType captureNameToHighlightType(const QString& captureName) const;
    
    TSParser* m_parser;
    QMap<openide::FileType, TSQuery*> m_queries;
};
}
#endif // TREESITTERWRAPPER_HPP


