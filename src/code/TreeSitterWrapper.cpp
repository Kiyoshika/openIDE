#include "code/TreeSitterWrapper.hpp"
#include <tree_sitter/api.h>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QHash>
#include <QDebug>

// Declare external language functions from tree-sitter parsers
extern "C" {
    const TSLanguage *tree_sitter_c();
    const TSLanguage *tree_sitter_cpp();
    const TSLanguage *tree_sitter_python();
    const TSLanguage *tree_sitter_java();
    const TSLanguage *tree_sitter_javascript();
    const TSLanguage *tree_sitter_typescript();
    const TSLanguage *tree_sitter_go();
    const TSLanguage *tree_sitter_rust();
    const TSLanguage *tree_sitter_c_sharp();
    const TSLanguage *tree_sitter_ruby();
    const TSLanguage *tree_sitter_php();
    const TSLanguage *tree_sitter_swift();
    const TSLanguage *tree_sitter_kotlin();
    const TSLanguage *tree_sitter_html();
    const TSLanguage *tree_sitter_css();
    // const TSLanguage *tree_sitter_sql();  // Skipped - no pre-generated parser
    const TSLanguage *tree_sitter_bash();
    const TSLanguage *tree_sitter_markdown();
    const TSLanguage *tree_sitter_json();
    const TSLanguage *tree_sitter_xml();
    const TSLanguage *tree_sitter_yaml();
}

using namespace openide::code;
using namespace openide;

TreeSitterWrapper::TreeSitterWrapper()
    : m_parser(nullptr)
{
    m_parser = ts_parser_new();
}

TreeSitterWrapper::~TreeSitterWrapper()
{
    // Clean up queries
    for (auto it = m_queries.begin(); it != m_queries.end(); ++it)
    {
        if (it.value())
        {
            ts_query_delete(it.value());
        }
    }
    m_queries.clear();
    
    // Clean up parser
    if (m_parser)
    {
        ts_parser_delete(m_parser);
    }
}

const TSLanguage* TreeSitterWrapper::getLanguage(FileType fileType) const
{
    switch (fileType)
    {
        case FileType::C: return tree_sitter_c();
        case FileType::CPP: return tree_sitter_cpp();
        case FileType::PYTHON: return tree_sitter_python();
        case FileType::JAVA: return tree_sitter_java();
        case FileType::JAVASCRIPT: return tree_sitter_javascript();
        case FileType::TYPESCRIPT: return tree_sitter_typescript();
        case FileType::GO: return tree_sitter_go();
        case FileType::RUST: return tree_sitter_rust();
        case FileType::CSHARP: return tree_sitter_c_sharp();
        case FileType::RUBY: return tree_sitter_ruby();
        case FileType::PHP: return tree_sitter_php();
        case FileType::SWIFT: return tree_sitter_swift();
        case FileType::KOTLIN: return tree_sitter_kotlin();
        case FileType::HTML: return tree_sitter_html();
        case FileType::CSS: return tree_sitter_css();
        case FileType::SQL: return nullptr;  // SQL parser not available (no pre-generated parser.c)
        case FileType::SHELL: return tree_sitter_bash();
        case FileType::MARKDOWN: return tree_sitter_markdown();
        case FileType::JSON: return tree_sitter_json();
        case FileType::XML: return tree_sitter_xml();
        case FileType::YAML: return tree_sitter_yaml();
        default: return nullptr;
    }
}

bool TreeSitterWrapper::isLanguageSupported(FileType fileType) const
{
    return getLanguage(fileType) != nullptr;
}

const char* TreeSitterWrapper::getQueryPatterns(FileType fileType) const
{
    // Map file types to their highlight query file paths
    static QHash<FileType, QString> queryFiles;
    static QHash<FileType, QByteArray> cachedQueries;
    
    if (queryFiles.isEmpty()) {
        // Get the source directory by going up from build directory
        // Build structure: build/Desktop_Qt_6_10_1_MinGW_64_bit-Debug/src/
        // We need to go up 3 levels to reach the project root
        QString appDir = QCoreApplication::applicationDirPath();
        QDir dir(appDir);
        dir.cdUp(); // Go to build/Desktop_Qt_6_10_1_MinGW_64_bit-Debug
        dir.cdUp(); // Go to build/
        dir.cdUp(); // Go to project root
        QString baseDir = dir.absolutePath() + "/external/";
        
        queryFiles[FileType::C] = baseDir + "tree-sitter-c/queries/highlights.scm";
        queryFiles[FileType::CPP] = baseDir + "tree-sitter-cpp/queries/highlights.scm";
        queryFiles[FileType::PYTHON] = baseDir + "tree-sitter-python/queries/highlights.scm";
        queryFiles[FileType::JAVA] = baseDir + "tree-sitter-java/queries/highlights.scm";
        queryFiles[FileType::JAVASCRIPT] = baseDir + "tree-sitter-javascript/queries/highlights.scm";
        queryFiles[FileType::TYPESCRIPT] = baseDir + "tree-sitter-typescript/queries/highlights.scm";
        queryFiles[FileType::GO] = baseDir + "tree-sitter-go/queries/highlights.scm";
        queryFiles[FileType::RUST] = baseDir + "tree-sitter-rust/queries/highlights.scm";
        queryFiles[FileType::CSHARP] = baseDir + "tree-sitter-c-sharp/queries/highlights.scm";
        queryFiles[FileType::RUBY] = baseDir + "tree-sitter-ruby/queries/highlights.scm";
        queryFiles[FileType::PHP] = baseDir + "tree-sitter-php/queries/highlights.scm";
        queryFiles[FileType::SWIFT] = baseDir + "tree-sitter-swift/queries/highlights.scm";
        queryFiles[FileType::KOTLIN] = baseDir + "tree-sitter-kotlin/queries/highlights.scm";
        queryFiles[FileType::HTML] = baseDir + "tree-sitter-html/queries/highlights.scm";
        queryFiles[FileType::CSS] = baseDir + "tree-sitter-css/queries/highlights.scm";
        queryFiles[FileType::SHELL] = baseDir + "tree-sitter-bash/queries/highlights.scm";
        queryFiles[FileType::MARKDOWN] = baseDir + "tree-sitter-markdown/tree-sitter-markdown/queries/highlights.scm";
        queryFiles[FileType::JSON] = baseDir + "tree-sitter-json/queries/highlights.scm";
        queryFiles[FileType::XML] = baseDir + "tree-sitter-xml/queries/highlights.scm";
        queryFiles[FileType::YAML] = baseDir + "tree-sitter-yaml/queries/highlights.scm";
    }
    
    // Check if we have a cached query for this file type
    if (cachedQueries.contains(fileType)) {
        return cachedQueries[fileType].constData();
    }
    
    // Try to load the query file
    QString filePath = queryFiles.value(fileType, "");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray content = file.readAll();
            file.close();
            
            // Append additional queries for specific languages to augment the official .scm
            if (fileType == FileType::CPP || fileType == FileType::C) {
                // Add essential missing patterns from C/C++ that aren't in the .scm
                content.append("\n; Additional essential patterns\n");
                content.append("(string_literal) @string\n");
                content.append("(system_lib_string) @string\n");
                content.append("(char_literal) @string\n");
                content.append("(number_literal) @number\n");
                content.append("(comment) @comment\n");
                content.append("(true) @constant\n");
                content.append("(false) @constant\n");
                content.append("(type_identifier) @type\n");
                content.append("(primitive_type) @type\n");
                content.append("(sized_type_specifier) @type\n");
                content.append("(call_expression function: (identifier) @function)\n");
            }
            
            cachedQueries[fileType] = content;
            qDebug() << "Loaded query file:" << filePath << "Size:" << content.size();
            return cachedQueries[fileType].constData();
        } else {
            qDebug() << "Failed to open query file:" << filePath;
        }
    } else {
        qDebug() << "No query file mapped for file type:" << static_cast<int>(fileType);
    }
    
    // Fallback to empty query if file not found
    return "";
}

QVector<HighlightInfo> TreeSitterWrapper::parseAndHighlight(const QString& text, FileType fileType)
{
    QVector<HighlightInfo> highlights;
    
    if (!isLanguageSupported(fileType))
    {
        return highlights;
    }
    
    const TSLanguage* language = getLanguage(fileType);
    if (!language)
    {
        return highlights;
    }
    
    // Set the language for the parser
    ts_parser_set_language(m_parser, language);
    
    // Parse the text
    QByteArray utf8Text = text.toUtf8();
    TSTree* tree = ts_parser_parse_string(m_parser, nullptr, utf8Text.constData(), utf8Text.length());
    
    if (!tree)
    {
        return highlights;
    }
    
    // Extract highlights using queries
    highlights = extractHighlights(text, tree, fileType);
    
    // Clean up
    ts_tree_delete(tree);
    
    return highlights;
}

QVector<HighlightInfo> TreeSitterWrapper::extractHighlights(const QString& text, TSTree* tree, FileType fileType)
{
    QVector<HighlightInfo> highlights;
    
    const char* querySource = getQueryPatterns(fileType);
    if (!querySource || querySource[0] == '\0')
    {
        return highlights;
    }
    
    // Get or create query for this file type
    TSQuery* query = m_queries.value(fileType, nullptr);
    if (!query)
    {
        uint32_t error_offset;
        TSQueryError error_type;
        const TSLanguage* language = getLanguage(fileType);
        
        query = ts_query_new(language, querySource, strlen(querySource), &error_offset, &error_type);
        
        if (!query || error_type != TSQueryErrorNone)
        {
            if (query)
            {
                ts_query_delete(query);
            }
            return highlights;
        }
        
        m_queries[fileType] = query;
    }
    
    // Create query cursor
    TSQueryCursor* cursor = ts_query_cursor_new();
    TSNode rootNode = ts_tree_root_node(tree);
    ts_query_cursor_exec(cursor, query, rootNode);
    
    // Extract matches
    TSQueryMatch match;
    while (ts_query_cursor_next_match(cursor, &match))
    {
        for (uint16_t i = 0; i < match.capture_count; i++)
        {
            TSQueryCapture capture = match.captures[i];
            TSNode node = capture.node;
            
            uint32_t start_byte = ts_node_start_byte(node);
            uint32_t end_byte = ts_node_end_byte(node);
            
            // Get capture name
            uint32_t length;
            const char* name = ts_query_capture_name_for_id(query, capture.index, &length);
            QString captureName = QString::fromUtf8(name, length);
            
            HighlightType type = captureNameToHighlightType(captureName);
            
            if (type != HighlightType::None)
            {
                HighlightInfo info;
                info.start = start_byte;
                info.length = end_byte - start_byte;
                info.type = type;
                highlights.append(info);
            }
        }
    }
    
    // Clean up
    ts_query_cursor_delete(cursor);
    
    return highlights;
}

HighlightType TreeSitterWrapper::captureNameToHighlightType(const QString& captureName) const
{
    if (captureName == "keyword") return HighlightType::Keyword;
    if (captureName == "comment") return HighlightType::Comment;
    if (captureName == "string") return HighlightType::String;
    if (captureName == "number") return HighlightType::Number;
    if (captureName == "function") return HighlightType::Function;
    if (captureName == "class") return HighlightType::Class;
    if (captureName == "type") return HighlightType::Type;
    if (captureName == "variable") return HighlightType::Variable;
    if (captureName == "operator") return HighlightType::Operator;
    if (captureName == "punctuation") return HighlightType::Punctuation;
    if (captureName == "property") return HighlightType::Property;
    if (captureName == "parameter") return HighlightType::Parameter;
    if (captureName == "constant") return HighlightType::Constant;
    
    return HighlightType::None;
}

