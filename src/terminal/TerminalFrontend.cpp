#include "terminal/TerminalFrontend.hpp"
#include "terminal/TerminalBackendInterface.hpp"
#include "MainWindow.hpp"
#include "AppSettings.hpp"
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
#include <QPushButton>
#include <QWheelEvent>

using namespace openide::terminal;

TerminalFrontend::TerminalFrontend(MainWindow* parent, TerminalBackendInterface* backend)
    : QAbstractScrollArea(parent->getCentralWidget())
    , m_mainWindow(parent)
    , m_backend(backend)
    , m_output()
    , m_currentDirectory()
    , m_inputLine(nullptr)
    , m_inputContainer(nullptr)
    , m_headerBar(nullptr)
    , m_collapseButton(nullptr)
    , m_closeButton(nullptr)
    , m_isCollapsed(false)
    , m_originalHeight(0)
    , m_fontSize(10)
{
    // Get initial font size from settings if available
    if (m_mainWindow && m_mainWindow->getAppSettings()) {
        int size = m_mainWindow->getAppSettings()->terminalFontSize();
        // Ensure font size is valid (between 6 and 72)
        if (size >= 6 && size <= 72) {
            m_fontSize = size;
        }
    }
    // Set background color for terminal
    setStyleSheet("background-color: #1e1e1e; color: #d4d4d4;");
    
    // Enable scrolling
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Get initial current directory
    m_currentDirectory = getCurrentDirectory();
    
    // Create header bar with buttons
    m_headerBar = new QWidget(this);
    m_headerBar->setStyleSheet("background-color: #252526; border-bottom: 1px solid #3e3e42;");
    m_headerBar->setFixedHeight(30);
    
    QHBoxLayout* headerLayout = new QHBoxLayout(m_headerBar);
    // Add top and right margins to prevent cutoff
    headerLayout->setContentsMargins(5, 2, 15, 2); // Right margin for scrollbar
    headerLayout->setSpacing(5);
    
    QLabel* titleLabel = new QLabel("Terminal", m_headerBar);
    titleLabel->setStyleSheet("color: #cccccc; background-color: transparent; font-weight: bold;");
    headerLayout->addWidget(titleLabel);
    
    headerLayout->addStretch();
    
    // Collapse/Expand button
    m_collapseButton = new QPushButton("−", m_headerBar); // Minus sign for collapse
    m_collapseButton->setFixedSize(24, 24);
    m_collapseButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #3e3e42;"
        "  color: #cccccc;"
        "  border: 1px solid #555555;"
        "  border-radius: 3px;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #4e4e52;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #2e2e32;"
        "}"
    );
    m_collapseButton->setToolTip("Collapse/Expand Terminal");
    connect(m_collapseButton, &QPushButton::clicked, this, &TerminalFrontend::toggleCollapse);
    headerLayout->addWidget(m_collapseButton);
    
    // Close button
    m_closeButton = new QPushButton("×", m_headerBar); // × symbol for close
    m_closeButton->setFixedSize(24, 24);
    m_closeButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #3e3e42;"
        "  color: #cccccc;"
        "  border: 1px solid #555555;"
        "  border-radius: 3px;"
        "  font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #c42b1c;"
        "  color: #ffffff;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #a02010;"
        "}"
    );
    m_closeButton->setToolTip("Close Terminal");
    connect(m_closeButton, &QPushButton::clicked, this, &TerminalFrontend::closeTerminal);
    headerLayout->addWidget(m_closeButton);
    
    m_headerBar->setLayout(headerLayout);
    m_headerBar->show();
    
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
    m_inputLine->setFont(QFont("Consolas", m_fontSize));
    m_inputLine->installEventFilter(this);
    
    inputLayout->addWidget(m_promptLabel);
    inputLayout->addWidget(m_inputLine, 1);
    
    m_inputContainer->setLayout(inputLayout);
    m_inputContainer->show();
    
    // Connect return key to execute command
    connect(m_inputLine, &QLineEdit::returnPressed, this, &TerminalFrontend::onCommandEntered);
    
    // Install event filter on viewport to catch wheel events for Ctrl+Scroll
    viewport()->installEventFilter(this);
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
        if (m_headerBar) {
            m_headerBar->setStyleSheet("background-color: #252526; border-bottom: 1px solid #3e3e42;");
        }
        if (m_collapseButton) {
            m_collapseButton->setStyleSheet(
                "QPushButton {"
                "  background-color: #3e3e42;"
                "  color: #cccccc;"
                "  border: 1px solid #555555;"
                "  border-radius: 3px;"
                "  font-size: 14px;"
                "}"
                "QPushButton:hover {"
                "  background-color: #4e4e52;"
                "}"
                "QPushButton:pressed {"
                "  background-color: #2e2e32;"
                "}"
            );
        }
        if (m_closeButton) {
            m_closeButton->setStyleSheet(
                "QPushButton {"
                "  background-color: #3e3e42;"
                "  color: #cccccc;"
                "  border: 1px solid #555555;"
                "  border-radius: 3px;"
                "  font-size: 16px;"
                "}"
                "QPushButton:hover {"
                "  background-color: #c42b1c;"
                "  color: #ffffff;"
                "}"
                "QPushButton:pressed {"
                "  background-color: #a02010;"
                "}"
            );
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
        if (m_headerBar) {
            m_headerBar->setStyleSheet("background-color: #f3f3f3; border-bottom: 1px solid #d0d0d0;");
        }
        if (m_collapseButton) {
            m_collapseButton->setStyleSheet(
                "QPushButton {"
                "  background-color: #e0e0e0;"
                "  color: #333333;"
                "  border: 1px solid #b0b0b0;"
                "  border-radius: 3px;"
                "  font-size: 14px;"
                "}"
                "QPushButton:hover {"
                "  background-color: #d0d0d0;"
                "}"
                "QPushButton:pressed {"
                "  background-color: #c0c0c0;"
                "}"
            );
        }
        if (m_closeButton) {
            m_closeButton->setStyleSheet(
                "QPushButton {"
                "  background-color: #e0e0e0;"
                "  color: #333333;"
                "  border: 1px solid #b0b0b0;"
                "  border-radius: 3px;"
                "  font-size: 16px;"
                "}"
                "QPushButton:hover {"
                "  background-color: #c42b1c;"
                "  color: #ffffff;"
                "}"
                "QPushButton:pressed {"
                "  background-color: #a02010;"
                "}"
            );
        }
    }
    // Force repaint
    update();
}

void TerminalFrontend::showEvent(QShowEvent* event)
{
    QAbstractScrollArea::showEvent(event);
    
    // Reload font size from settings when terminal is shown
    // This ensures the font size is up-to-date if it was changed via Ctrl+Scroll
    if (m_mainWindow && m_mainWindow->getAppSettings()) {
        int size = m_mainWindow->getAppSettings()->terminalFontSize();
        if (size >= 6 && size <= 72 && size != m_fontSize) {
            updateFontSize(size);
        }
    }
    
    // CRITICAL: Ensure header bar is visible and properly positioned when terminal is shown
    // This is especially important when terminal was collapsed before being hidden
    if (m_headerBar) {
        m_headerBar->show();
        m_headerBar->raise(); // Bring to front
        // Reposition header bar with top margin and account for scrollbar
        int scrollbarWidth = verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0;
        m_headerBar->move(0, 2); // Top margin
        m_headerBar->resize(width() - scrollbarWidth, 30);
        // Explicitly show buttons
        if (m_collapseButton) {
            m_collapseButton->show();
        }
        if (m_closeButton) {
            m_closeButton->show();
        }
    }
    
    // If collapsed, ensure the header bar is still accessible
    if (m_isCollapsed && m_headerBar) {
        m_headerBar->show();
        m_headerBar->raise();
    }
    
    // Initialize terminal when first shown
    if (m_output.isEmpty() && m_backend) {
        init();
    }
}

void TerminalFrontend::resizeEvent(QResizeEvent* event)
{
    QAbstractScrollArea::resizeEvent(event);
    
    // Position header bar at the top with margins - CRITICAL: always visible when terminal is visible
    // This must happen even when collapsed, so buttons remain accessible
    if (m_headerBar) {
        // Add top margin (2px) and account for scrollbar width on the right
        int scrollbarWidth = verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0;
        m_headerBar->move(0, 2); // Top margin
        m_headerBar->resize(width() - scrollbarWidth, 30);
        m_headerBar->show();
        m_headerBar->raise(); // Ensure it's on top of everything
        // Explicitly show buttons to ensure they're accessible
        if (m_collapseButton) {
            m_collapseButton->show();
        }
        if (m_closeButton) {
            m_closeButton->show();
        }
    }
    
    // Position input container at the bottom (only when not collapsed)
    if (m_inputContainer && !m_isCollapsed) {
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
    
    // Handle wheel events on viewport for Ctrl+Scroll font size adjustment
    if (obj == viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        if (wheelEvent->modifiers() & Qt::ControlModifier) {
            // Get the scroll direction
            int delta = wheelEvent->angleDelta().y();
            
            if (delta != 0) {
                // Use stored MainWindow pointer
                if (m_mainWindow && m_mainWindow->getAppSettings()) {
                    openide::AppSettings* settings = m_mainWindow->getAppSettings();
                    int currentSize = settings->terminalFontSize();
                    
                    // Adjust size based on scroll direction
                    // Clamp between reasonable bounds (6-72)
                    int newSize = currentSize;
                    if (delta > 0) {
                        newSize = qMin(72, currentSize + 1);  // Scroll up = increase
                    } else {
                        newSize = qMax(6, currentSize - 1);   // Scroll down = decrease
                    }
                    
                    // Only update if size actually changed
                    if (newSize != currentSize) {
                        // Save the new font size to settings and persist to file
                        settings->setTerminalFontSize(newSize);
                        settings->saveToFile();
                        
                        // Update stored font size and trigger repaint
                        updateFontSize(newSize);
                    }
                }
            }
            
            wheelEvent->accept();
            return true; // Event handled
        }
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
    
    // Account for header bar at top (30px height + 2px top margin = 32px)
    int headerHeight = m_headerBar ? 32 : 0;
    
    int inputHeight = m_inputContainer ? m_inputContainer->height() : 0;
    int availableHeight = viewportRect.height() - inputHeight - headerHeight;
    
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
        m_promptLabel->setText(m_currentDirectory + "> ");
    }
}

void TerminalFrontend::toggleCollapse()
{
    m_isCollapsed = !m_isCollapsed;
    
    if (m_isCollapsed) {
        // Store original height if not already stored
        if (m_originalHeight == 0) {
            m_originalHeight = height();
        }
        // Collapse to just show header bar
        setMaximumHeight(30);
        setMinimumHeight(30);
        // Hide input and viewport content
        if (m_inputContainer) {
            m_inputContainer->hide();
        }
        viewport()->hide();
        // Ensure header bar is visible and on top when collapsed
        if (m_headerBar) {
            m_headerBar->show();
            m_headerBar->raise();
        }
        if (m_collapseButton) {
            m_collapseButton->setText("+"); // Plus sign for expand
        }
    } else {
        // Restore original height
        setMaximumHeight(QWIDGETSIZE_MAX);
        setMinimumHeight(0);
        // Show input and viewport content
        if (m_inputContainer) {
            m_inputContainer->show();
        }
        viewport()->show();
        // Ensure header bar is visible when expanded
        if (m_headerBar) {
            m_headerBar->show();
            m_headerBar->raise();
        }
        if (m_collapseButton) {
            m_collapseButton->setText("−"); // Minus sign for collapse
        }
    }
    
    update();
}

void TerminalFrontend::closeTerminal()
{
    setVisible(false);
    // Reset collapse state when closing
    m_isCollapsed = false;
    m_originalHeight = 0;
    if (m_collapseButton) {
        m_collapseButton->setText("−");
    }
    if (m_inputContainer) {
        m_inputContainer->show();
    }
    viewport()->show();
    setMaximumHeight(QWIDGETSIZE_MAX);
    setMinimumHeight(0);
    // Header bar will be shown again when terminal becomes visible (in showEvent)
}

void TerminalFrontend::forceOpen()
{
    // If collapsed, expand it first
    if (m_isCollapsed) {
        toggleCollapse();
    }
    
    // Force show the terminal
    setVisible(true);
    
    // Ensure header bar is visible and properly positioned
    if (m_headerBar) {
        m_headerBar->show();
        m_headerBar->raise();
        // Position with top margin and account for scrollbar
        int scrollbarWidth = verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0;
        m_headerBar->move(0, 2); // Top margin
        m_headerBar->resize(width() - scrollbarWidth, 30);
    }
    
    // Ensure buttons are visible
    if (m_collapseButton) {
        m_collapseButton->show();
    }
    if (m_closeButton) {
        m_closeButton->show();
    }
    
    // Ensure viewport and input are visible
    if (m_inputContainer) {
        m_inputContainer->show();
    }
    viewport()->show();
    
    // Reset height constraints to allow full expansion
    setMaximumHeight(QWIDGETSIZE_MAX);
    setMinimumHeight(0);
}

void TerminalFrontend::updateHeaderButtons()
{
    if (m_collapseButton) {
        m_collapseButton->setText(m_isCollapsed ? "+" : "−");
    }
}

void TerminalFrontend::paintEvent(QPaintEvent* event)
{
    // Don't paint content if collapsed
    if (m_isCollapsed) {
        return;
    }
    
    QPainter painter(viewport());
    
    // Set font for terminal output - use stored font size
    QFont font("Consolas", m_fontSize);
    painter.setFont(font);
    
    QRect viewportRect = viewport()->rect();
    QFontMetrics fm(font);
    int lineHeight = fm.height();
    
    // Account for header bar at top (30px height + 2px top margin = 32px offset)
    int headerHeight = m_headerBar ? 32 : 0;
    
    // Account for input container at bottom
    int inputHeight = m_inputContainer ? m_inputContainer->height() : 0;
    // Available height excludes header and input
    int availableHeight = viewportRect.height() - inputHeight - headerHeight;
    
    // Account for scrollbar width on the right
    int scrollbarWidth = verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0;
    int availableWidth = viewportRect.width() - scrollbarWidth - 10; // 10px right margin
    
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
        // Start drawing below the header bar (account for header height + top margin)
        int y = headerHeight + lineHeight - scrollOffset;
        int x = 10; // Left margin
        
        // Get text color based on theme
        QColor normalTextColor;
        QPalette palette = this->palette();
        normalTextColor = palette.color(QPalette::Text);
        
        // Split output into lines and draw each line
        QStringList lines = m_output.split('\n');
        for (const QString& line : lines) {
            // Only draw if line is within visible area (accounting for header)
            if (y + lineHeight >= headerHeight && y <= availableHeight + headerHeight) {
                // Use red color for error messages, yellow for warnings, theme-appropriate color for regular output
                if (line.contains("Error:", Qt::CaseInsensitive)) {
                    painter.setPen(QColor(244, 67, 54)); // Red for errors
                } else if (line.contains("Warning:", Qt::CaseInsensitive)) {
                    painter.setPen(QColor(255, 193, 7)); // Yellow/amber for warnings
                } else {
                    painter.setPen(normalTextColor); // Use theme-appropriate text color
                }
                // Clip text to available width to prevent cutoff from scrollbar
                QString clippedLine = fm.elidedText(line, Qt::ElideRight, availableWidth);
                painter.drawText(x, y, clippedLine);
            }
            y += lineHeight;
        }
    }
    // Don't draw anything when output is empty - keep it blank
}

void TerminalFrontend::updateFontSize(int size)
{
    if (size >= 6 && size <= 72) {
        m_fontSize = size;
        if (m_inputLine) {
            m_inputLine->setFont(QFont("Consolas", m_fontSize));
        }
        update(); // Trigger repaint with new font size
    }
}

void TerminalFrontend::wheelEvent(QWheelEvent* event)
{
    // Handle Ctrl+scroll for font size adjustment
    if (event->modifiers() & Qt::ControlModifier) {
        // Get the scroll direction
        int delta = event->angleDelta().y();
        
        if (delta != 0) {
            // Get MainWindow from parent hierarchy
            QWidget* centralWidget = qobject_cast<QWidget*>(parent());
            if (centralWidget) {
                MainWindow* mainWindow = qobject_cast<MainWindow*>(centralWidget->parent());
                if (mainWindow && mainWindow->getAppSettings()) {
                    openide::AppSettings* settings = mainWindow->getAppSettings();
                    int currentSize = settings->terminalFontSize();
                    
                    // Adjust size based on scroll direction
                    // Clamp between reasonable bounds (6-72)
                    int newSize = currentSize;
                    if (delta > 0) {
                        newSize = qMin(72, currentSize + 1);  // Scroll up = increase
                    } else {
                        newSize = qMax(6, currentSize - 1);   // Scroll down = decrease
                    }
                    
                    // Only update if size actually changed
                    if (newSize != currentSize) {
                        // Save the new font size to settings and persist to file
                        settings->setTerminalFontSize(newSize);
                        settings->saveToFile();
                        
                        // Update stored font size and trigger repaint
                        updateFontSize(newSize);
                    }
                }
            }
            
            event->accept();
            return;
        }
    }
    
    // For non-Ctrl scroll, use default behavior
    QAbstractScrollArea::wheelEvent(event);
}
