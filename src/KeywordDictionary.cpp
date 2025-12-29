#include "KeywordDictionary.hpp"

using namespace openide;

// TODO: it'd be much nicer to have this as a file config to read from, but
// for now just having code-defined keywords to get started

static QStringList cKeywords = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "inline", "int", "long", "register", "restrict", "return", "short", "signed",
    "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void",
    "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex",
    "_Generic", "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local"
};

static QStringList cppKeywords = {
    "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor",
    "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
    "class", "compl", "concept", "const", "consteval", "constexpr", "constinit",
    "const_cast", "continue", "co_await", "co_return", "co_yield", "decltype",
    "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit",
    "export", "extern", "false", "float", "for", "friend", "goto", "if",
    "inline", "int", "long", "mutable", "namespace", "new", "noexcept", "not",
    "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected", "public",
    "register", "reinterpret_cast", "requires", "return", "short", "signed", "sizeof",
    "static", "static_assert", "static_cast", "struct", "switch", "template", "this",
    "thread_local", "throw", "true", "try", "typedef", "typeid", "typename", "union",
    "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
};

static QStringList javaKeywords = {
    "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
    "class", "const", "continue", "default", "do", "double", "else", "enum",
    "extends", "final", "finally", "float", "for", "goto", "if", "implements",
    "import", "instanceof", "int", "interface", "long", "native", "new", "package",
    "private", "protected", "public", "return", "short", "static", "strictfp", "super",
    "switch", "synchronized", "this", "throw", "throws", "transient", "try", "void",
    "volatile", "while", "true", "false", "null", "var", "yield", "record", "sealed",
    "permits", "non-sealed"
};

static QStringList pythonKeywords = {
    "False", "None", "True", "and", "as", "assert", "async", "await",
    "break", "class", "continue", "def", "del", "elif", "else", "except",
    "finally", "for", "from", "global", "if", "import", "in", "is",
    "lambda", "nonlocal", "not", "or", "pass", "raise", "return", "try",
    "while", "with", "yield", "self", "cls", "__init__", "__main__"
};

static QStringList javascriptKeywords = {
    "abstract", "arguments", "await", "boolean", "break", "byte", "case", "catch",
    "char", "class", "const", "continue", "debugger", "default", "delete", "do",
    "double", "else", "enum", "eval", "export", "extends", "false", "final",
    "finally", "float", "for", "function", "goto", "if", "implements", "import",
    "in", "instanceof", "int", "interface", "let", "long", "native", "new",
    "null", "package", "private", "protected", "public", "return", "short", "static",
    "super", "switch", "synchronized", "this", "throw", "throws", "transient", "true",
    "try", "typeof", "var", "void", "volatile", "while", "with", "yield",
    "async", "of", "from"
};

static QStringList typescriptKeywords = {
    "abstract", "any", "as", "asserts", "async", "await", "bigint", "boolean",
    "break", "case", "catch", "class", "const", "constructor", "continue", "debugger",
    "declare", "default", "delete", "do", "else", "enum", "export", "extends",
    "false", "finally", "for", "from", "function", "get", "if", "implements",
    "import", "in", "infer", "instanceof", "interface", "is", "keyof", "let",
    "module", "namespace", "never", "new", "null", "number", "object", "of",
    "package", "private", "protected", "public", "readonly", "require", "return",
    "set", "static", "string", "super", "switch", "symbol", "this", "throw",
    "true", "try", "type", "typeof", "undefined", "unique", "unknown", "var",
    "void", "while", "with", "yield"
};

static QStringList goKeywords = {
    "break", "case", "chan", "const", "continue", "default", "defer", "else",
    "fallthrough", "for", "func", "go", "goto", "if", "import", "interface",
    "map", "package", "range", "return", "select", "struct", "switch", "type",
    "var", "bool", "byte", "complex64", "complex128", "error", "float32", "float64",
    "int", "int8", "int16", "int32", "int64", "rune", "string", "uint",
    "uint8", "uint16", "uint32", "uint64", "uintptr", "true", "false", "iota",
    "nil", "append", "cap", "close", "complex", "copy", "delete", "imag",
    "len", "make", "new", "panic", "print", "println", "real", "recover"
};

static QStringList rustKeywords = {
    "as", "async", "await", "break", "const", "continue", "crate", "dyn",
    "else", "enum", "extern", "false", "fn", "for", "if", "impl",
    "in", "let", "loop", "match", "mod", "move", "mut", "pub",
    "ref", "return", "Self", "self", "static", "struct", "super", "trait",
    "true", "type", "unsafe", "use", "where", "while", "abstract", "become",
    "box", "do", "final", "macro", "override", "priv", "try", "typeof",
    "unsized", "virtual", "yield", "union", "i8", "i16", "i32", "i64",
    "i128", "u8", "u16", "u32", "u64", "u128", "f32", "f64",
    "isize", "usize", "bool", "char", "str", "String", "Vec", "Option",
    "Result", "Some", "None", "Ok", "Err"
};

static QStringList csharpKeywords = {
    "abstract", "as", "base", "bool", "break", "byte", "case", "catch",
    "char", "checked", "class", "const", "continue", "decimal", "default", "delegate",
    "do", "double", "else", "enum", "event", "explicit", "extern", "false",
    "finally", "fixed", "float", "for", "foreach", "goto", "if", "implicit",
    "in", "int", "interface", "internal", "is", "lock", "long", "namespace",
    "new", "null", "object", "operator", "out", "override", "params", "private",
    "protected", "public", "readonly", "ref", "return", "sbyte", "sealed", "short",
    "sizeof", "stackalloc", "static", "string", "struct", "switch", "this", "throw",
    "true", "try", "typeof", "uint", "ulong", "unchecked", "unsafe", "ushort",
    "using", "virtual", "void", "volatile", "while", "add", "alias", "ascending",
    "async", "await", "by", "descending", "dynamic", "equals", "from", "get",
    "global", "group", "into", "join", "let", "nameof", "on", "orderby",
    "partial", "remove", "select", "set", "unmanaged", "value", "var", "when",
    "where", "yield", "record", "init", "with", "nint", "nuint"
};

static QStringList rubyKeywords = {
    "BEGIN", "END", "alias", "and", "begin", "break", "case", "class",
    "def", "defined?", "do", "else", "elsif", "end", "ensure", "false",
    "for", "if", "in", "module", "next", "nil", "not", "or",
    "redo", "rescue", "retry", "return", "self", "super", "then", "true",
    "undef", "unless", "until", "when", "while", "yield", "__FILE__", "__LINE__",
    "attr_accessor", "attr_reader", "attr_writer", "private", "protected", "public",
    "require", "include", "extend", "raise", "puts", "print", "p", "gets"
};

static QStringList phpKeywords = {
    "abstract", "and", "array", "as", "break", "callable", "case", "catch",
    "class", "clone", "const", "continue", "declare", "default", "die", "do",
    "echo", "else", "elseif", "empty", "enddeclare", "endfor", "endforeach", "endif",
    "endswitch", "endwhile", "eval", "exit", "extends", "final", "finally", "fn",
    "for", "foreach", "function", "global", "goto", "if", "implements", "include",
    "include_once", "instanceof", "insteadof", "interface", "isset", "list", "match",
    "namespace", "new", "or", "print", "private", "protected", "public", "readonly",
    "require", "require_once", "return", "static", "switch", "throw", "trait", "try",
    "unset", "use", "var", "while", "xor", "yield", "yield from", "true", "false", "null"
};

static QStringList swiftKeywords = {
    "associatedtype", "class", "deinit", "enum", "extension", "fileprivate", "func", "import",
    "init", "inout", "internal", "let", "open", "operator", "private", "precedencegroup",
    "protocol", "public", "rethrows", "static", "struct", "subscript", "typealias", "var",
    "break", "case", "catch", "continue", "default", "defer", "do", "else",
    "fallthrough", "for", "guard", "if", "in", "repeat", "return", "throw",
    "switch", "where", "while", "as", "false", "is", "nil", "self",
    "Self", "super", "throws", "true", "try", "Any", "async", "await",
    "some", "actor", "isolated"
};

static QStringList kotlinKeywords = {
    "abstract", "actual", "annotation", "as", "break", "by", "catch", "class",
    "companion", "const", "constructor", "continue", "crossinline", "data", "delegate", "do",
    "dynamic", "else", "enum", "expect", "external", "false", "field", "final",
    "finally", "for", "fun", "get", "if", "import", "in", "infix",
    "init", "inline", "inner", "interface", "internal", "is", "lateinit", "noinline",
    "null", "object", "open", "operator", "out", "override", "package", "param",
    "private", "property", "protected", "public", "receiver", "reified", "return", "sealed",
    "set", "setparam", "super", "suspend", "tailrec", "this", "throw", "true",
    "try", "typealias", "typeof", "val", "var", "vararg", "when", "where", "while"
};

static QStringList htmlKeywords = {
    "a", "abbr", "address", "area", "article", "aside", "audio", "b",
    "base", "bdi", "bdo", "blockquote", "body", "br", "button", "canvas",
    "caption", "cite", "code", "col", "colgroup", "data", "datalist", "dd",
    "del", "details", "dfn", "dialog", "div", "dl", "dt", "em",
    "embed", "fieldset", "figcaption", "figure", "footer", "form", "h1", "h2",
    "h3", "h4", "h5", "h6", "head", "header", "hgroup", "hr",
    "html", "i", "iframe", "img", "input", "ins", "kbd", "label",
    "legend", "li", "link", "main", "map", "mark", "meta", "meter",
    "nav", "noscript", "object", "ol", "optgroup", "option", "output", "p",
    "param", "picture", "pre", "progress", "q", "rp", "rt", "ruby",
    "s", "samp", "script", "section", "select", "small", "source", "span",
    "strong", "style", "sub", "summary", "sup", "svg", "table", "tbody",
    "td", "template", "textarea", "tfoot", "th", "thead", "time", "title",
    "tr", "track", "u", "ul", "var", "video", "wbr"
};

static QStringList cssKeywords = {
    "align-content", "align-items", "align-self", "all", "animation", "animation-delay",
    "animation-direction", "animation-duration", "animation-fill-mode", "animation-iteration-count",
    "animation-name", "animation-play-state", "animation-timing-function", "backface-visibility",
    "background", "background-attachment", "background-blend-mode", "background-clip",
    "background-color", "background-image", "background-origin", "background-position",
    "background-repeat", "background-size", "border", "border-bottom", "border-bottom-color",
    "border-bottom-left-radius", "border-bottom-right-radius", "border-bottom-style",
    "border-bottom-width", "border-collapse", "border-color", "border-image", "border-left",
    "border-radius", "border-right", "border-spacing", "border-style", "border-top",
    "border-width", "bottom", "box-shadow", "box-sizing", "break-after", "break-before",
    "break-inside", "caption-side", "clear", "clip", "color", "column-count",
    "column-fill", "column-gap", "column-rule", "column-span", "column-width", "columns",
    "content", "counter-increment", "counter-reset", "cursor", "direction", "display",
    "empty-cells", "filter", "flex", "flex-basis", "flex-direction", "flex-flow",
    "flex-grow", "flex-shrink", "flex-wrap", "float", "font", "font-family",
    "font-size", "font-style", "font-variant", "font-weight", "gap", "grid",
    "grid-area", "grid-auto-columns", "grid-auto-flow", "grid-auto-rows", "grid-column",
    "grid-column-end", "grid-column-start", "grid-gap", "grid-row", "grid-row-end",
    "grid-row-start", "grid-template", "grid-template-areas", "grid-template-columns",
    "grid-template-rows", "height", "justify-content", "justify-items", "justify-self",
    "left", "letter-spacing", "line-height", "list-style", "list-style-image",
    "list-style-position", "list-style-type", "margin", "margin-bottom", "margin-left",
    "margin-right", "margin-top", "max-height", "max-width", "min-height", "min-width",
    "object-fit", "object-position", "opacity", "order", "outline", "outline-color",
    "outline-offset", "outline-style", "outline-width", "overflow", "overflow-x",
    "overflow-y", "padding", "padding-bottom", "padding-left", "padding-right",
    "padding-top", "page-break-after", "page-break-before", "page-break-inside",
    "perspective", "pointer-events", "position", "quotes", "resize", "right",
    "text-align", "text-decoration", "text-indent", "text-overflow", "text-shadow",
    "text-transform", "top", "transform", "transform-origin", "transform-style",
    "transition", "transition-delay", "transition-duration", "transition-property",
    "transition-timing-function", "vertical-align", "visibility", "white-space",
    "width", "word-break", "word-spacing", "word-wrap", "z-index"
};

static QStringList sqlKeywords = {
    "ADD", "ALL", "ALTER", "AND", "ANY", "AS", "ASC", "BACKUP",
    "BETWEEN", "CASE", "CHECK", "COLUMN", "CONSTRAINT", "CREATE", "DATABASE", "DEFAULT",
    "DELETE", "DESC", "DISTINCT", "DROP", "EXEC", "EXISTS", "FOREIGN", "FROM",
    "FULL", "GROUP", "HAVING", "IN", "INDEX", "INNER", "INSERT", "INTO",
    "IS", "JOIN", "KEY", "LEFT", "LIKE", "LIMIT", "NOT", "NULL",
    "OR", "ORDER", "OUTER", "PRIMARY", "PROCEDURE", "RIGHT", "ROWNUM", "SELECT",
    "SET", "TABLE", "TOP", "TRUNCATE", "UNION", "UNIQUE", "UPDATE", "VALUES",
    "VIEW", "WHERE", "AVG", "COUNT", "MAX", "MIN", "SUM", "BEGIN",
    "COMMIT", "ROLLBACK", "TRANSACTION", "TRIGGER", "GRANT", "REVOKE", "VARCHAR",
    "INT", "INTEGER", "BOOLEAN", "DATE", "DATETIME", "TIMESTAMP", "TEXT", "BLOB"
};

static QStringList shellKeywords = {
    "if", "then", "else", "elif", "fi", "case", "esac", "for",
    "select", "while", "until", "do", "done", "in", "function", "time",
    "coproc", "break", "continue", "return", "exit", "shift", "export", "readonly",
    "declare", "typeset", "local", "unset", "trap", "source", "eval", "exec",
    "echo", "printf", "read", "test", "let", "expr", "true", "false",
    "alias", "unalias", "set", "unset", "cd", "pwd", "pushd", "popd",
    "dirs", "bg", "fg", "jobs", "kill", "wait", "suspend", "ulimit"
};


QStringList KeywordDictionary::getKeywordsForFileType(enum FileType fileType)
{
    switch (fileType)
    {
        case FileType::C: { return cKeywords; }
        case FileType::CPP: { return cppKeywords; }
        case FileType::JAVA: { return javaKeywords; }
        case FileType::PYTHON: { return pythonKeywords; }
        case FileType::JAVASCRIPT: { return javascriptKeywords; }
        case FileType::TYPESCRIPT: { return typescriptKeywords; }
        case FileType::GO: { return goKeywords; }
        case FileType::RUST: { return rustKeywords; }
        case FileType::CSHARP: { return csharpKeywords; }
        case FileType::RUBY: { return rubyKeywords; }
        case FileType::PHP: { return phpKeywords; }
        case FileType::SWIFT: { return swiftKeywords; }
        case FileType::KOTLIN: { return kotlinKeywords; }
        case FileType::HTML: { return htmlKeywords; }
        case FileType::CSS: { return cssKeywords; }
        case FileType::SQL: { return sqlKeywords; }
        case FileType::SHELL: { return shellKeywords; }
        case FileType::MARKDOWN: { return {}; }
        case FileType::JSON: { return {}; }
        case FileType::XML: { return {}; }
        case FileType::YAML: { return {}; }
        case FileType::UNKNOWN: { return {}; }
    }

    return {};
}
