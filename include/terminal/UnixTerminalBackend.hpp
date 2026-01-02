#ifndef UNIXTERMINALBACKEND_HPP
#define UNIXTERMINALBACKEND_HPP

#ifndef WIN32
#include "TerminalBackendInterface.hpp"
#include <QString>
#include <QProcess>

namespace openide::terminal
{
struct UnixTerminalBackend : public TerminalBackendInterface
{
    UnixTerminalBackend();
    virtual ~UnixTerminalBackend();
    
    void init() override;
    void close() override;
    QString executeCommand(const QString& command, const QString& workingDirectory = QString()) override;
    
private:
    QString findShell();
    QProcess* m_process;
    QString m_shellPath;
};
}
#endif // !WIN32
#endif // UNIXTERMINALBACKEND_HPP

