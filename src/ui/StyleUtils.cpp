#include "ui/StyleUtils.hpp"

using namespace openide::ui;

QString StyleUtils::getSplitterHandleStyle(bool isDarkTheme)
{
    if (isDarkTheme) {
        // Dark theme: lighter handle to distinguish from dark background (#353535)
        return "QSplitter::handle { background-color: #4a4a4a; }";
    } else {
        // Light theme: slightly darker handle to distinguish from light background (#ffffff)
        return "QSplitter::handle { background-color: #d0d0d0; }";
    }
}

