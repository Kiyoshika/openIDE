#ifndef TERMINALFRONTEND_HPP
#define TERMINALFRONTEND_HPP

namespace openide::terminal
{
class TerminalFrontend : public QAbstractScrollArea
{
Q_OBJECT
public:
	TerminalFrontend();
    ~TerminalFrontend() = default;
private:
};
} // namespace openide::terminal
#endif