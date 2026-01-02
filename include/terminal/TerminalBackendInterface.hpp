#ifndef TERMINALBACKENDINTERFACE_HPP
#define TERMINALBACKENDINTERFACE_HPP

#include <QString>
#include <QProcess>

namespace openide::terminal
{
struct TerminalBackendInterface
{
public:
    TerminalBackendInterface();
    virtual ~TerminalBackendInterface() = default;
    
    virtual void init() = 0;
    virtual void close();
    virtual QString executeCommand(const QString& command, const QString& workingDirectory = QString()) = 0;
    
protected:
    // Common output cleaning function - removes ANSI escape sequences and normalizes output
    static QString cleanTerminalOutput(const QString& output);
    
    // Platform-specific output cleaning - can be overridden by derived classes
    virtual QString platformSpecificCleanOutput(const QString& output);
    
    // Common helper to execute a process and return output
    QString executeProcess(const QString& program, const QStringList& arguments, const QString& workingDirectory);
    
    // Common members
    bool m_isInitialized;
    QString m_workingDirectory;
};
}
#endif // TERMINALBACKENDINTERFACE_HPP
