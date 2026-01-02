#ifndef TERMINALBACKENDINTERFACE_HPP
#define TERMINALBACKENDINTERFACE_HPP

#include <QString>

namespace openide::terminal
{
struct TerminalBackendInterface
{
public:
    virtual ~TerminalBackendInterface() = default;
    virtual void init() = 0;
    virtual QString executeCommand(const QString& command, const QString& workingDirectory = QString()) = 0;
};
}
#endif // TERMINALBACKENDINTERFACE_HPP
