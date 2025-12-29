#include "FileType.hpp"

using namespace openide;

enum FileType FileTypeUtil::fromExtension(const QString& fileExtension)
{
    if (fileExtension == "c" || fileExtension == "h") return FileType::C;
    if (fileExtension == "cpp" || fileExtension == "cxx" || fileExtension == "cc" || 
        fileExtension == "hpp" || fileExtension == "hxx" || fileExtension == "hh") return FileType::CPP;
    if (fileExtension == "java") return FileType::JAVA;
    if (fileExtension == "py" || fileExtension == "pyw") return FileType::PYTHON;
    if (fileExtension == "js" || fileExtension == "jsx" || fileExtension == "mjs") return FileType::JAVASCRIPT;
    if (fileExtension == "ts" || fileExtension == "tsx") return FileType::TYPESCRIPT;
    if (fileExtension == "go") return FileType::GO;
    if (fileExtension == "rs") return FileType::RUST;
    if (fileExtension == "cs") return FileType::CSHARP;
    if (fileExtension == "rb") return FileType::RUBY;
    if (fileExtension == "php") return FileType::PHP;
    if (fileExtension == "swift") return FileType::SWIFT;
    if (fileExtension == "kt" || fileExtension == "kts") return FileType::KOTLIN;
    if (fileExtension == "html" || fileExtension == "htm") return FileType::HTML;
    if (fileExtension == "css" || fileExtension == "scss" || fileExtension == "sass" || fileExtension == "less") return FileType::CSS;
    if (fileExtension == "sql") return FileType::SQL;
    if (fileExtension == "sh" || fileExtension == "bash" || fileExtension == "zsh") return FileType::SHELL;
    if (fileExtension == "md" || fileExtension == "markdown") return FileType::MARKDOWN;
    if (fileExtension == "json") return FileType::JSON;
    if (fileExtension == "xml") return FileType::XML;
    if (fileExtension == "yaml" || fileExtension == "yml") return FileType::YAML;

    return FileType::UNKNOWN;
}
