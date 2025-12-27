#include "KeywordDictionary.hpp"

using namespace openide;

// TODO: it'd be much nicer to have this as a file config to read from, but
// for now just having code-defined keywords to get started
static QStringList cKeywords = { "int" };
static QStringList cppKeywords = { "int", "namespace", "class", "const", "public", "private" };
static QStringList javaKeywords = {};
static QStringList pythonKeywords = {};


QStringList KeywordDictionary::getKeywordsForFileType(enum FileType fileType)
{
    switch (fileType)
    {
        case FileType::C: { return cKeywords; }
        case FileType::CPP: { return cppKeywords; }
        case FileType::JAVA: { return javaKeywords; }
        case FileType::PYTHON: { return pythonKeywords; }
        case FileType::UNKNOWN: { return {}; }
    }

    return {};
}
