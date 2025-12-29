#ifndef FILETYPE_HPP
#define FILETYPE_HPP

#include <QString>

namespace openide
{
enum class FileType
{
    C,
    CPP,
    JAVA,
    PYTHON,
    UNKNOWN
};

struct FileTypeUtil
{
    static enum FileType fromExtension(const QString& fileExtension);
};
}

#endif // FILETYPE_HPP
