#ifndef TERMINALFRONTEND_HPP
#define TERMINALFRONTEND_HPP

#include <QAbstractScrollArea>

// forward decl
class MainWindow;

namespace openide::terminal
{
class TerminalFrontend : public QAbstractScrollArea
{
Q_OBJECT
public:
    TerminalFrontend(MainWindow* parent);
    ~TerminalFrontend() = default;
private:
};
} // namespace openide::terminal
#endif
