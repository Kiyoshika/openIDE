#ifdef WIN32
#include "terminal/WindowsTerminalBackend.hpp"
#include <QString>
#include <QRegularExpression>

using namespace openide::terminal;

// Constructor: Initialize process
WindowsTerminalBackend::WindowsTerminalBackend()
    : TerminalBackendInterface()
    , m_process(nullptr)
{
}

// Destructor: Ensure cleanup
WindowsTerminalBackend::~WindowsTerminalBackend()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
    if (m_process) {
        delete m_process;
        m_process = nullptr;
    }
    // Set initialized state directly (avoiding virtual call)
    m_isInitialized = false;
}

// Initialize - no-op for QProcess approach
void WindowsTerminalBackend::init()
{
    m_isInitialized = true;
}

// Close - no-op for QProcess approach
void WindowsTerminalBackend::close()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
    TerminalBackendInterface::close();
}

// Platform-specific output cleaning - remove PowerShell artifacts
QString WindowsTerminalBackend::platformSpecificCleanOutput(const QString& output)
{
    QString cleaned = output;
    
    // Remove PowerShell-specific artifacts: return code and executable path
    // Remove patterns: digits + semicolon + full PowerShell path
    QRegularExpression powershellArtifact1(R"([\d]+;[/\\]?[^;\n\r]*[Ww]indows[Pp]ower[Ss]hell[/\\]v[\d.]+[/\\]powershell\.exe?)", QRegularExpression::CaseInsensitiveOption);
    cleaned.remove(powershellArtifact1);
    
    // Remove patterns: digits + semicolon + any path ending in powershell.exe (fallback for other locations)
    QRegularExpression powershellArtifact2(R"([\d]+;[/\\]?[^;\n\r]*powershell\.exe?)", QRegularExpression::CaseInsensitiveOption);
    cleaned.remove(powershellArtifact2);
    
    return cleaned;
}

// Execute a command and return output
QString WindowsTerminalBackend::executeCommand(const QString& command, const QString& workingDirectory)
{
    // Set up the process to run PowerShell with the command
    // Use -NoProfile -NonInteractive -Command to execute the command and suppress extra output
    QString program = "powershell.exe";
    QStringList arguments;
    arguments << "-NoProfile" << "-NonInteractive" << "-Command" << command;
    
    // Use the common executeProcess helper
    return executeProcess(program, arguments, workingDirectory);
}

#endif // WIN32
