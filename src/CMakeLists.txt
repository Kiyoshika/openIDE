set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

# Tree-sitter core source files (explicitly listed)
set(TS_CORE_DIR ${CMAKE_SOURCE_DIR}/external/tree-sitter/lib/src)

set(OPENIDE_CORE core)

qt_add_library(${OPENIDE_CORE} SHARED
    # OpenIDE source files
    code/CodeEditor.cpp
    code/CodeTabPane.cpp
    code/SyntaxHighlighter.cpp
    code/FindReplaceDialog.cpp
    code/TreeSitterWrapper.cpp
    code/ColorScheme.cpp
    menu/FileMenu.cpp
    menu/EditMenu.cpp
    menu/ThemeMenu.cpp
    menu/SettingsMenu.cpp
    menu/SettingsDialog.cpp
    menu/NewProjectDialog.cpp
    menu/NewFileDialog.cpp
    AppSettings.cpp
    FileType.cpp
    ProjectTree.cpp
    MainWindow.cpp
    # Tree-sitter core C files
    ${TS_CORE_DIR}/alloc.c
    ${TS_CORE_DIR}/get_changed_ranges.c
    ${TS_CORE_DIR}/language.c
    ${TS_CORE_DIR}/lexer.c
    ${TS_CORE_DIR}/node.c
    ${TS_CORE_DIR}/parser.c
    ${TS_CORE_DIR}/point.c
    ${TS_CORE_DIR}/query.c
    ${TS_CORE_DIR}/stack.c
    ${TS_CORE_DIR}/subtree.c
    ${TS_CORE_DIR}/tree_cursor.c
    ${TS_CORE_DIR}/tree.c
    ${TS_CORE_DIR}/wasm_store.c
    # Parser files
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-c/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-cpp/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-cpp/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-python/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-python/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-java/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-javascript/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-javascript/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-typescript/typescript/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-typescript/typescript/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-go/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-rust/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-rust/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-c-sharp/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-c-sharp/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-ruby/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-ruby/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-php/php/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-php/php/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-swift/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-kotlin/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-kotlin/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-html/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-html/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-css/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-css/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-bash/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-bash/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-markdown/tree-sitter-markdown/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-markdown/tree-sitter-markdown/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-json/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-xml/xml/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-xml/xml/src/scanner.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-yaml/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-yaml/src/scanner.c
    # Headers
    ../include/MainWindow.hpp ../include/code/CodeEditor.hpp ../include/code/CodeTabPane.hpp ../include/code/FindReplaceDialog.hpp ../include/code/TreeSitterWrapper.hpp ../include/code/ColorScheme.hpp ../include/menu/FileMenu.hpp ../include/menu/EditMenu.hpp ../include/menu/ThemeMenu.hpp ../include/menu/SettingsMenu.hpp ../include/menu/SettingsDialog.hpp ../include/menu/NewProjectDialog.hpp ../include/menu/NewFileDialog.hpp ../include/AppSettings.hpp ../include/FileType.hpp ../include/ProjectTree.hpp ../include/code/SyntaxHighlighter.hpp
)

# Remove the target_sources line as all sources are now in qt_add_library

# For parser files: prepend their own src directory to include path so they use bundled parser.h
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-c/src/parser.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-c/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-cpp/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-cpp/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-cpp/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-python/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-python/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-python/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-java/src/parser.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-java/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-javascript/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-javascript/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-javascript/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-typescript/typescript/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-typescript/typescript/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-typescript/typescript/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-go/src/parser.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-go/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-rust/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-rust/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-rust/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-c-sharp/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-c-sharp/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-c-sharp/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-ruby/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-ruby/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-ruby/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-php/php/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-php/php/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-php/php/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-swift/src/parser.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-swift/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-kotlin/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-kotlin/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-kotlin/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-html/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-html/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-html/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-css/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-css/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-css/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-bash/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-bash/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-bash/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-markdown/tree-sitter-markdown/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-markdown/tree-sitter-markdown/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-markdown/tree-sitter-markdown/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-json/src/parser.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-json/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-xml/xml/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-xml/xml/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-xml/xml/src"
)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-yaml/src/parser.c
    ${CMAKE_SOURCE_DIR}/external/tree-sitter-yaml/src/scanner.c
    PROPERTIES COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/external/tree-sitter-yaml/src"
)

# Set include directories for tree-sitter core files to use the central parser.h
set_source_files_properties(
    ${TS_CORE_DIR}/alloc.c
    ${TS_CORE_DIR}/get_changed_ranges.c
    ${TS_CORE_DIR}/language.c
    ${TS_CORE_DIR}/lexer.c
    ${TS_CORE_DIR}/node.c
    ${TS_CORE_DIR}/parser.c
    ${TS_CORE_DIR}/point.c
    ${TS_CORE_DIR}/query.c
    ${TS_CORE_DIR}/stack.c
    ${TS_CORE_DIR}/subtree.c
    ${TS_CORE_DIR}/tree_cursor.c
    ${TS_CORE_DIR}/tree.c
    ${TS_CORE_DIR}/wasm_store.c
    PROPERTIES
    INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/external/tree-sitter/lib/include;${CMAKE_SOURCE_DIR}/external/tree-sitter/lib/src"
)

# Add compile definitions for tree-sitter to build without ICU
# Also add typedef for parser version compatibility
target_compile_definitions(${OPENIDE_CORE} PRIVATE
    TREE_SITTER_HIDE_SYMBOLS
    TSFieldMapSlice=TSMapSlice
)

target_include_directories(${OPENIDE_CORE} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/tree-sitter/lib/include
    ${CMAKE_SOURCE_DIR}/external/tree-sitter/lib/src
)

# Disable warnings for tree-sitter C files
set_source_files_properties(${TREE_SITTER_SOURCES} ${PARSER_SOURCES} PROPERTIES COMPILE_FLAGS "-w")

target_link_libraries(${OPENIDE_CORE} PUBLIC 
    Qt${QT_VERSION_MAJOR}::Widgets 
    Qt${QT_VERSION_MAJOR}::Core
)

# On Windows with MinGW, export all symbols from the shared library
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(${OPENIDE_CORE} PRIVATE -Wl,--export-all-symbols)
endif()

set(PROJECT_SOURCES
        main.cpp
)
qt_add_executable(${CMAKE_PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE 
    ${OPENIDE_CORE} 
    Qt6::Widgets
)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include)
qt_finalize_executable(${CMAKE_PROJECT_NAME})
#target_compile_options(openIDE PRIVATE -fsanitize=address -fno-omit-frame-pointer)
#target_link_options(openIDE PRIVATE -fsanitize=address)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${CMAKE_PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

