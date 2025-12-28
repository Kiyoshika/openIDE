set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_library(openide SHARED
    code/CodeEditor.cpp
    code/CodeTabPane.cpp
    menu/FileMenu.cpp
    FileType.cpp
    KeywordDictionary.cpp
    ProjectTree.cpp
    SyntaxHighlighter.cpp
    ../include/code/CodeEditor.hpp ../include/code/CodeTabPane.hpp ../include/menu/FileMenu.hpp ../include/FileType.hpp ../include/KeywordDictionary.hpp ../include/MainWindow.hpp ../include/ProjectTree.hpp ../include/SyntaxHighlighter.hpp
)
target_include_directories(openide PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(openide PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        ../include/MainWindow.hpp
)
qt_add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE openide Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include)
qt_finalize_executable(openIDE)
#target_compile_options(openIDE PRIVATE -fsanitize=address -fno-omit-frame-pointer)
#target_link_options(openIDE PRIVATE -fsanitize=address)

set_target_properties(openIDE PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS openIDE
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

