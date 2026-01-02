#ifndef STYLEUTILS_HPP
#define STYLEUTILS_HPP

#include <QString>

namespace openide::ui
{
/**
 * Utility functions for UI styling
 */
namespace StyleUtils
{
    /**
     * Get the stylesheet for QSplitter handles based on theme
     * @param isDarkTheme true for dark theme, false for light theme
     * @return QString containing the stylesheet for QSplitter::handle
     */
    QString getSplitterHandleStyle(bool isDarkTheme);
}
}

#endif // STYLEUTILS_HPP

