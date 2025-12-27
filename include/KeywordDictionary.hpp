#ifndef KEYWORDDICTIONARY_HPP
#define KEYWORDDICTIONARY_HPP

#include "FileType.hpp"

#include <QStringList>

namespace openide
{
struct KeywordDictionary
{
    static QStringList getKeywordsForFileType(enum FileType fileType);
};
}
#endif // KEYWORDDICTIONARY_HPP
