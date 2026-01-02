#ifndef WINDOWSTERMINALBACKEND_HPP
#define WINDOWSTERMINALBACKEND_HPP

#ifdef WIN32
#include "TerminalBackendInterface.hpp"
#include <QString>
#include <QProcess>

namespace openide::terminal
{
struct WindowsTerminalBackend : public TerminalBackendInterface
{
    WindowsTerminalBackend();
    virtual ~WindowsTerminalBackend();
    
    void init() override;
    void close();
    QString executeCommand(const QString& command, const QString& workingDirectory = QString());
    
private:
    QProcess* m_process;
    bool m_isInitialized;
    QString m_workingDirectory;
};
}
#endif // WIN32
#endif // WINDOWSTERMINALBACKEND_HPP
