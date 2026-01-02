#ifndef WIN32
#include "terminal/UnixTerminalBackend.hpp"
#include <QString>
#include <QFileInfo>

using namespace openide::terminal;

// Constructor: Initialize process
UnixTerminalBackend::UnixTerminalBackend()
    : TerminalBackendInterface()
    , m_process(nullptr)
    , m_shellPath()
{
    // Find available shell
    m_shellPath = findShell();
}

// Destructor: Ensure cleanup
UnixTerminalBackend::~UnixTerminalBackend()
{
    close();
    if (m_process) {
        delete m_process;
        m_process = nullptr;
    }
}

// Find an available shell (bash, zsh, sh) with fallbacks
QString UnixTerminalBackend::findShell()
{
    // List of shells to try in order of preference
    QStringList shells = {
        "/bin/bash",
        "/bin/zsh",
        "/usr/bin/bash",
        "/usr/bin/zsh",
        "/bin/sh"
    };
    
    // Also check if SHELL environment variable is set
    QString envShell = QString::fromLocal8Bit(qgetenv("SHELL"));
    if (!envShell.isEmpty() && QFileInfo::exists(envShell)) {
        // Prefer user's default shell
        return envShell;
    }
    
    // Try each shell in order
    for (const QString& shell : shells) {
        if (QFileInfo::exists(shell)) {
            return shell;
        }
    }
    
    // Fallback: try to find any shell in common locations
    // This is a last resort
    return "/bin/sh";  // POSIX sh should always exist on Unix systems
}

// Initialize - no-op for QProcess approach
void UnixTerminalBackend::init()
{
    // Verify shell is still available
    if (m_shellPath.isEmpty() || !QFileInfo::exists(m_shellPath)) {
        m_shellPath = findShell();
    }
    m_isInitialized = true;
}

// Close - no-op for QProcess approach
void UnixTerminalBackend::close()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
    TerminalBackendInterface::close();
}

// Execute a command and return output
QString UnixTerminalBackend::executeCommand(const QString& command, const QString& workingDirectory)
{
    // Verify shell is still available
    if (m_shellPath.isEmpty() || !QFileInfo::exists(m_shellPath)) {
        m_shellPath = findShell();
        if (m_shellPath.isEmpty()) {
            return QString("Error: No shell found. Please ensure bash, zsh, or sh is installed.");
        }
    }
    
    // Determine shell name for -c flag
    QString shellName = QFileInfo(m_shellPath).fileName();
    
    // Set up the process to run the shell with the command
    // Use -c to execute the command as a string
    QString program = m_shellPath;
    QStringList arguments;
    
    // Different shells may need different flags
    if (shellName == "bash" || shellName == "zsh") {
        // bash and zsh support -c
        arguments << "-c" << command;
    } else {
        // Fallback for sh and other POSIX shells
        arguments << "-c" << command;
    }
    
    // Use the common executeProcess helper
    return executeProcess(program, arguments, workingDirectory);
}

#endif // !WIN32

