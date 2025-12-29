#include "FileType.hpp"

using namespace openide;

enum FileType FileTypeUtil::fromExtension(const QString& fileExtension)
{
    if (fileExtension == "c") return FileType::C;
    if (fileExtension == "cpp" || fileExtension == "cxx" || fileExtension == "cc") return FileType::CPP;
    if (fileExtension == "java") return FileType::JAVA;
    if (fileExtension == "py") return FileType::PYTHON;

    return FileType::UNKNOWN;
}
