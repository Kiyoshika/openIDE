#include "terminal/TerminalBackendInterface.hpp"
#include <QRegularExpression>
#include <QDir>

using namespace openide::terminal;

// Constructor: Initialize common members
TerminalBackendInterface::TerminalBackendInterface()
    : m_isInitialized(false)
    , m_workingDirectory()
{
}

// Close - default implementation
void TerminalBackendInterface::close()
{
    m_isInitialized = false;
}

// Common output cleaning function - removes ANSI escape sequences and control characters
QString TerminalBackendInterface::cleanTerminalOutput(const QString& output)
{
    if (output.isEmpty()) {
        return output;
    }
    
    QString cleaned;
    cleaned.reserve(output.length());
    
    bool inEscapeSequence = false;
    bool inCsiSequence = false; // Control Sequence Introducer
    
    for (int i = 0; i < output.length(); ++i) {
        QChar c = output[i];
        unsigned short ch = c.unicode();
        
        // Handle ANSI escape sequences
        if (inEscapeSequence) {
            if (ch == '[') {
                inCsiSequence = true;
                continue;
            } else if (inCsiSequence) {
                // CSI sequence: ESC[ followed by parameters and a command character
                // Command characters are typically in range 0x40-0x7E
                if (ch >= 0x40 && ch <= 0x7E) {
                    // End of CSI sequence
                    inEscapeSequence = false;
                    inCsiSequence = false;
                }
                // Skip all characters in the CSI sequence
                continue;
            } else if (ch == 0x1B) {
                // Another escape character, continue
                continue;
            } else if ((ch >= 0x40 && ch <= 0x5F) || (ch >= 0x60 && ch <= 0x7E)) {
                // Two-character escape sequence (ESC followed by a command character)
                inEscapeSequence = false;
                continue;
            } else {
                // Continue in escape sequence
                continue;
            }
        }
        
        // Check for escape character (0x1B or \033)
        if (ch == 0x1B) {
            inEscapeSequence = true;
            continue;
        }
        
        // Filter control characters, but keep useful ones
        if (ch < 0x20) {
            // Keep newline, carriage return, and tab
            if (ch == '\n' || ch == '\r' || ch == '\t') {
                cleaned.append(c);
            }
            // Skip all other control characters (BEL, BS, etc.)
            continue;
        }
        
        // Remove DEL character
        if (ch == 0x7F) {
            continue;
        }
        
        // Keep printable ASCII characters (0x20-0x7E) and valid UTF-8 sequences
        // For extended characters, we'll keep them as they might be valid UTF-8
        if (ch >= 0x20 && ch <= 0x7E) {
            // Standard ASCII printable
            cleaned.append(c);
        } else if (ch > 0x7F) {
            // Extended character - keep it (might be UTF-8)
            cleaned.append(c);
        }
    }
    
    // Normalize line endings (convert \r\n to \n, standalone \r to \n)
    cleaned.replace("\r\n", "\n");
    cleaned.replace('\r', '\n');
    
    // Remove excessive blank lines (more than 2 consecutive newlines)
    while (cleaned.contains("\n\n\n")) {
        cleaned.replace("\n\n\n", "\n\n");
    }
    
    // Trim leading and trailing whitespace, but preserve structure
    cleaned = cleaned.trimmed();
    
    return cleaned;
}

// Platform-specific output cleaning - default implementation does nothing
QString TerminalBackendInterface::platformSpecificCleanOutput(const QString& output)
{
    return output;
}

// Common helper to execute a process and return output
QString TerminalBackendInterface::executeProcess(const QString& program, const QStringList& arguments, const QString& workingDirectory)
{
    if (!m_isInitialized) {
        return QString("Error: Terminal not initialized. Call init() first.");
    }
    
    // Create a new QProcess for this command
    QProcess process;
    
    // Set the working directory if provided
    QString actualWorkingDir = workingDirectory;
    if (actualWorkingDir.isEmpty() && !m_workingDirectory.isEmpty()) {
        actualWorkingDir = m_workingDirectory;
    }
    
    if (!actualWorkingDir.isEmpty()) {
        process.setWorkingDirectory(actualWorkingDir);
        m_workingDirectory = actualWorkingDir;
    }
    
    // Set process channel mode to merge stdout and stderr
    process.setProcessChannelMode(QProcess::MergedChannels);
    
    // Start the process
    process.start(program, arguments);
    
    // Wait for the process to finish (with timeout)
    if (!process.waitForFinished(30000)) {
        // Process timed out or failed to start
        if (process.state() == QProcess::Running) {
            process.kill();
            process.waitForFinished(1000);
        }
        return QString("Error: Command execution timed out or failed to start.");
    }
    
    // Read the output
    QByteArray output = process.readAllStandardOutput();
    QString result;
    
    // Use UTF-8 for Unix, Local8Bit for Windows
#ifdef WIN32
    result = QString::fromLocal8Bit(output);
#else
    result = QString::fromUtf8(output);
#endif
    
    // Clean the output using common cleaning
    result = cleanTerminalOutput(result);
    
    // Apply platform-specific cleaning
    result = platformSpecificCleanOutput(result);
    
    return result;
}

