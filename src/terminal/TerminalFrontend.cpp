#include "terminal/TerminalFrontend.hpp"
#include "terminal/TerminalBackendInterface.hpp"
#include "terminal/WindowsTerminalBackend.hpp"
#include "MainWindow.hpp"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QShowEvent>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QDir>
#include <QResizeEvent>
#include <QLabel>
#include <QScrollBar>
#ifdef WIN32
#include <windows.h>
#endif

using namespace openide::terminal;

TerminalFrontend::TerminalFrontend(MainWindow* parent, TerminalBackendInterface* backend)
    : QAbstractScrollArea(parent->getCentralWidget())
    , m_backend(backend)
    , m_output()
    , m_currentDirectory()
    , m_inputLine(nullptr)
    , m_inputContainer(nullptr)
{
    // Set background color for terminal
    setStyleSheet("background-color: #1e1e1e; color: #d4d4d4;");
    
    // Enable scrolling
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Get initial current directory
    m_currentDirectory = getCurrentDirectory();
    
    // Create input line at the bottom
    m_inputContainer = new QWidget(viewport());
    m_inputContainer->setStyleSheet("background-color: #1e1e1e;");
    
    QHBoxLayout* inputLayout = new QHBoxLayout(m_inputContainer);
    inputLayout->setContentsMargins(10, 0, 10, 5);
    inputLayout->setSpacing(5);
    
    // Create prompt label
    m_promptLabel = new QLabel(m_inputContainer);
    m_promptLabel->setStyleSheet("color: #4ec9b0; background-color: transparent;");
    updatePrompt();
    m_promptLabel->setMinimumWidth(200);
    
    m_inputLine = new QLineEdit(m_inputContainer);
    m_inputLine->setStyleSheet("background-color: #252526; color: #d4d4d4; border: 1px solid #3e3e42; padding: 2px;");
    m_inputLine->setFont(QFont("Consolas", 10));
    m_inputLine->installEventFilter(this);
    
    inputLayout->addWidget(m_promptLabel);
    inputLayout->addWidget(m_inputLine, 1);
    
    m_inputContainer->setLayout(inputLayout);
    m_inputContainer->show();
    
    // Connect return key to execute command
    connect(m_inputLine, &QLineEdit::returnPressed, this, &TerminalFrontend::onCommandEntered);
}

void TerminalFrontend::setBackend(TerminalBackendInterface* backend)
{
    m_backend = backend;
}

void TerminalFrontend::init()
{
    if (!m_backend) {
        appendOutput("Error: No terminal backend available for this operating system.\n");
        return;
    }
    
    // Initialize the backend
    m_backend->init();
    
    // Show welcome message
    appendOutput("Terminal ready. Type commands and press Enter to execute.\n");
    updatePrompt();
}

void TerminalFrontend::updateTheme(bool isDarkTheme)
{
    if (isDarkTheme) {
        // Dark theme colors
        setStyleSheet("background-color: #1e1e1e; color: #d4d4d4;");
        if (m_inputContainer) {
            m_inputContainer->setStyleSheet("background-color: #1e1e1e;");
        }
        if (m_inputLine) {
            m_inputLine->setStyleSheet("background-color: #252526; color: #d4d4d4; border: 1px solid #3e3e42; padding: 2px;");
        }
        if (m_promptLabel) {
            m_promptLabel->setStyleSheet("color: #4ec9b0; background-color: transparent;");
        }
    } else {
        // Light theme colors
        setStyleSheet("background-color: #ffffff; color: #000000;");
        if (m_inputContainer) {
            m_inputContainer->setStyleSheet("background-color: #ffffff;");
        }
        if (m_inputLine) {
            m_inputLine->setStyleSheet("background-color: #f3f3f3; color: #000000; border: 1px solid #d0d0d0; padding: 2px;");
        }
        if (m_promptLabel) {
            m_promptLabel->setStyleSheet("color: #0078d4; background-color: transparent;");
        }
    }
    // Force repaint
    update();
}

void TerminalFrontend::showEvent(QShowEvent* event)
{
    QAbstractScrollArea::showEvent(event);
    
    // Initialize terminal when first shown
    if (m_output.isEmpty() && m_backend) {
        init();
    }
}

void TerminalFrontend::resizeEvent(QResizeEvent* event)
{
    QAbstractScrollArea::resizeEvent(event);
    
    // Position input container at the bottom
    if (m_inputContainer) {
        QRect rect = viewport()->rect();
        int inputHeight = m_inputContainer->sizeHint().height();
        m_inputContainer->setGeometry(0, rect.height() - inputHeight, rect.width(), inputHeight);
    }
}

bool TerminalFrontend::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_inputLine && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        // Handle special keys if needed
        return QAbstractScrollArea::eventFilter(obj, event);
    }
    return QAbstractScrollArea::eventFilter(obj, event);
}

void TerminalFrontend::onCommandEntered()
{
    QString command = m_inputLine->text().trimmed();
    if (command.isEmpty()) {
        return;
    }
    
    // Clear input
    m_inputLine->clear();
    
    // Execute the command
    executeUserCommand(command);
}

void TerminalFrontend::executeUserCommand(const QString& command)
{
    if (!m_backend) {
        appendOutput("Error: No terminal backend available.\n");
        return;
    }
    
    // Show the command with prompt
    appendOutput(m_currentDirectory + "> " + command + "\n");
    
#ifdef WIN32
    WindowsTerminalBackend* windowsBackend = dynamic_cast<WindowsTerminalBackend*>(m_backend);
    if (windowsBackend) {
        // Handle cd command specially to update current directory
        if (command.startsWith("cd ", Qt::CaseInsensitive)) {
            QString newDir = command.mid(3).trimmed();
            if (newDir.isEmpty()) {
                // cd without arguments - go to home
                newDir = QDir::homePath();
            } else if (newDir == "..") {
                // Go up one directory
                QDir dir(m_currentDirectory);
                dir.cdUp();
                newDir = dir.absolutePath();
            } else if (!QDir(newDir).isAbsolute()) {
                // Relative path
                QDir dir(m_currentDirectory);
                if (dir.cd(newDir)) {
                    newDir = dir.absolutePath();
                } else {
                    appendOutput("Error: Directory not found: " + newDir + "\n");
                    updatePrompt();
                    return;
                }
            }
            
            // Update current directory
            QDir dir(newDir);
            if (dir.exists()) {
                m_currentDirectory = dir.absolutePath();
                updatePrompt();
            } else {
                appendOutput("Error: Directory not found: " + newDir + "\n");
            }
        } else {
            // Execute the command with the current working directory
            QString result = windowsBackend->executeCommand(command, m_currentDirectory);
            
            // Append the result (even if empty, to show command was executed)
            if (!result.isEmpty()) {
                appendOutput(result);
                if (!result.endsWith('\n')) {
                    appendOutput("\n");
                }
            } else {
                // No output - might be a silent command, add a newline for spacing
                appendOutput("\n");
            }
            
            // Don't reset the directory - keep the tracked directory
            // Only update if the command actually changed directories (handled by cd command)
            updatePrompt();
        }
    } else {
        appendOutput("Error: Failed to cast backend to WindowsTerminalBackend.\n");
    }
#else
    appendOutput("Error: WindowsTerminalBackend is only available on Windows.\n");
#endif
}

void TerminalFrontend::appendOutput(const QString& text)
{
    m_output += text;
    update();
    
    // Auto-scroll to bottom
    QScrollBar* vbar = verticalScrollBar();
    if (vbar) {
        vbar->setValue(vbar->maximum());
    }
}

QString TerminalFrontend::getCurrentDirectory() const
{
#ifdef WIN32
    wchar_t buffer[MAX_PATH];
    DWORD length = GetCurrentDirectoryW(MAX_PATH, buffer);
    if (length > 0 && length < MAX_PATH) {
        return QString::fromWCharArray(buffer, length);
    }
#endif
    // Fallback to QDir
    return QDir::currentPath();
}

void TerminalFrontend::updatePrompt()
{
    if (m_promptLabel) {
        m_promptLabel->setText(m_currentDirectory + ">");
    }
}

void TerminalFrontend::paintEvent(QPaintEvent* event)
{
    QPainter painter(viewport());
    
    // Set font for terminal output
    QFont font("Consolas", 10);
    painter.setFont(font);
    
    QRect viewportRect = viewport()->rect();
    QFontMetrics fm(font);
    int lineHeight = fm.height();
    
    // Account for input container at bottom
    int inputHeight = m_inputContainer ? m_inputContainer->height() : 0;
    int availableHeight = viewportRect.height() - inputHeight;
    
    // Calculate total content height for scrolling
    int totalLines = 0;
    if (!m_output.isEmpty()) {
        totalLines = m_output.split('\n').count();
    } else {
        totalLines = 1; // Placeholder text
    }
    int totalContentHeight = totalLines * lineHeight + 20; // Add some padding
    
    // Set up scroll area range
    QScrollBar* vbar = verticalScrollBar();
    if (vbar) {
        vbar->setRange(0, qMax(0, totalContentHeight - availableHeight));
        vbar->setPageStep(availableHeight);
        vbar->setSingleStep(lineHeight);
    }
    
    // Get the scroll offset
    int scrollOffset = vbar ? vbar->value() : 0;
    
    // Draw the output text
    if (!m_output.isEmpty()) {
        int y = lineHeight - scrollOffset;
        int x = 10;
        
        // Split output into lines and draw each line
        QStringList lines = m_output.split('\n');
        for (const QString& line : lines) {
            // Only draw if line is within visible area
            if (y + lineHeight >= 0 && y <= availableHeight) {
                // Use red color for error messages, yellow for warnings, normal gray for regular output
                if (line.contains("Error:", Qt::CaseInsensitive)) {
                    painter.setPen(QColor(244, 67, 54)); // Red for errors
                } else if (line.contains("Warning:", Qt::CaseInsensitive)) {
                    painter.setPen(QColor(255, 193, 7)); // Yellow/amber for warnings
                } else {
                    painter.setPen(QColor(212, 212, 212)); // Light gray for normal output
                }
                painter.drawText(x, y, line);
            }
            y += lineHeight;
        }
    } else {
        // Draw placeholder text if no output
        int y = 20 - scrollOffset;
        if (y >= 0 && y <= availableHeight) {
            painter.setPen(QColor(212, 212, 212)); // Light gray
            painter.drawText(10, y, "Terminal ready...");
        }
    }
}
