#ifndef FILETYPE_HPP
#define FILETYPE_HPP

#include <QString>
#include <QMetaType>

namespace openide
{
enum class FileType
{
    C,
    CPP,
    JAVA,
    PYTHON,
    JAVASCRIPT,
    TYPESCRIPT,
    GO,
    RUST,
    CSHARP,
    RUBY,
    PHP,
    SWIFT,
    KOTLIN,
    HTML,
    CSS,
    SQL,
    SHELL,
    MARKDOWN,
    JSON,
    XML,
    YAML,
    UNKNOWN
};

struct FileTypeUtil
{
    static enum FileType fromExtension(const QString& fileExtension);
};
}

// Register the enum with Qt's meta-type system so it can be used in signals/slots
Q_DECLARE_METATYPE(openide::FileType)

#endif // FILETYPE_HPP
