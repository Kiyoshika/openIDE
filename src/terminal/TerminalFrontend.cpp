#include "terminal/TerminalFrontend.hpp"
#include "terminal/TerminalBackendInterface.hpp"
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

// Helper function to expand tilde (~) to home directory
static QString expandTilde(const QString& path)
{
    if (path.startsWith("~/")) {
        // Replace ~/ with home path
        return QDir::homePath() + path.mid(1);
    } else if (path == "~") {
        // Just ~ means home directory
        return QDir::homePath();
    } else if (path.startsWith("~")) {
        // Handle ~username or other tilde patterns
        // For now, just expand ~ to home (more complex expansion can be added later)
        return QDir::homePath() + path.mid(1);
    }
    return path;
}

void TerminalFrontend::executeUserCommand(const QString& command)
{
    if (!m_backend) {
        appendOutput("Error: No terminal backend available.\n");
        return;
    }
    
    // Handle clear command specially to clear the output (before showing the command)
    if (command.compare("clear", Qt::CaseInsensitive) == 0) {
        m_output.clear();
        update();
        // Reset scroll position
        QScrollBar* vbar = verticalScrollBar();
        if (vbar) {
            vbar->setValue(0);
        }
        return;
    }
    
    // Show the command with prompt
    appendOutput(m_currentDirectory + "> " + command + "\n");
    
    // Handle cd command specially to update current directory
    if (command.startsWith("cd ", Qt::CaseInsensitive)) {
        QString newDir = command.mid(3).trimmed();
        if (newDir.isEmpty()) {
            // cd without arguments - go to home
            newDir = QDir::homePath();
        } else {
            // Expand tilde if present
            newDir = expandTilde(newDir);
            
            if (newDir == "..") {
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
        // Expand tilde in the command before executing
        QString expandedCommand = command;
        QString homePath = QDir::homePath();
        
        // Replace ~/ with home path (handles cases like "ls ~/Documents" or "cat ~/file.txt")
        expandedCommand.replace("~/", homePath + "/");
        
        // Replace standalone ~ at start of command or after space (handles "ls ~" or "echo ~")
        // Use word boundaries to avoid replacing ~ in the middle of other text
        if (expandedCommand.startsWith("~")) {
            // ~ at the start
            if (expandedCommand.length() == 1 || expandedCommand.at(1).isSpace() || expandedCommand.at(1) == '/') {
                expandedCommand.replace(0, 1, homePath);
            }
        } else {
            // ~ after a space
            expandedCommand.replace(QRegularExpression(R"((\s)~(\s|/|$))"), "\\1" + homePath + "\\2");
        }
        
        // Execute the command with the current working directory using the interface
        QString result = m_backend->executeCommand(expandedCommand, m_currentDirectory);
        
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
}

void TerminalFrontend::appendOutput(const QString& text)
{
    m_output += text;
    
    // Calculate scroll range before updating
    QFont font("Consolas", 10);
    QFontMetrics fm(font);
    int lineHeight = fm.height();
    QRect viewportRect = viewport()->rect();
    int inputHeight = m_inputContainer ? m_inputContainer->height() : 0;
    int availableHeight = viewportRect.height() - inputHeight;
    
    int totalLines = 0;
    if (!m_output.isEmpty()) {
        totalLines = m_output.split('\n').count();
    }
    // No placeholder text - keep it blank when empty
    int totalContentHeight = totalLines * lineHeight + 20;
    
    // Update scroll range
    QScrollBar* vbar = verticalScrollBar();
    if (vbar) {
        vbar->setRange(0, qMax(0, totalContentHeight - availableHeight));
        vbar->setPageStep(availableHeight);
        vbar->setSingleStep(lineHeight);
        
        // Auto-scroll to bottom
        vbar->setValue(vbar->maximum());
    }
    
    update();
}

QString TerminalFrontend::getCurrentDirectory() const
{
    // Use QDir for cross-platform directory retrieval
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
    }
    // No placeholder text - keep it blank when empty
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
        
        // Get text color based on theme
        QColor normalTextColor;
        QPalette palette = this->palette();
        normalTextColor = palette.color(QPalette::Text);
        
        // Split output into lines and draw each line
        QStringList lines = m_output.split('\n');
        for (const QString& line : lines) {
            // Only draw if line is within visible area
            if (y + lineHeight >= 0 && y <= availableHeight) {
                // Use red color for error messages, yellow for warnings, theme-appropriate color for regular output
                if (line.contains("Error:", Qt::CaseInsensitive)) {
                    painter.setPen(QColor(244, 67, 54)); // Red for errors
                } else if (line.contains("Warning:", Qt::CaseInsensitive)) {
                    painter.setPen(QColor(255, 193, 7)); // Yellow/amber for warnings
                } else {
                    painter.setPen(normalTextColor); // Use theme-appropriate text color
                }
                painter.drawText(x, y, line);
            }
            y += lineHeight;
        }
    }
    // Don't draw anything when output is empty - keep it blank
}
