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
    void close() override;
    QString executeCommand(const QString& command, const QString& workingDirectory = QString()) override;
    
protected:
    QString platformSpecificCleanOutput(const QString& output) override;
    
private:
    QProcess* m_process;
};
}
#endif // WIN32
#endif // WINDOWSTERMINALBACKEND_HPP
