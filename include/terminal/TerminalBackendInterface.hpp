#ifndef TERMINALBACKENDINTERFACE_HPP
#define TERMINALBACKENDINTERFACE_HPP

namespace openide::terminal
{
struct TerminalBackendInterface
{
public:
    virtual void init() = 0;
};
}
#endif // TERMINALBACKENDINTERFACE_HPP
