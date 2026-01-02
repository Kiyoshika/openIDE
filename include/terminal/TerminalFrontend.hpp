#ifndef TERMINALFRONTEND_HPP
#define TERMINALFRONTEND_HPP

#include <QAbstractScrollArea>
#include <QString>
#include <QPaintEvent>
#include <QShowEvent>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

// forward decl
class MainWindow;

namespace openide::terminal
{
struct TerminalBackendInterface;

class TerminalFrontend : public QAbstractScrollArea
{
Q_OBJECT
public:
    TerminalFrontend(MainWindow* parent, TerminalBackendInterface* backend = nullptr);
    ~TerminalFrontend() = default;
    void setBackend(TerminalBackendInterface* backend);
    void init();
    void updateTheme(bool isDarkTheme);
    bool isCollapsed() const { return m_isCollapsed; }
    void forceOpen(); // Force terminal to be visible and expanded
    
public slots:
    void toggleCollapse();
    void closeTerminal();
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    
private slots:
    void onCommandEntered();
    
private:
    void executeUserCommand(const QString& command);
    void appendOutput(const QString& text);
    QString getCurrentDirectory() const;
    void updatePrompt();
    
    void updateHeaderButtons();
    
    TerminalBackendInterface* m_backend;
    QString m_output;
    QString m_currentDirectory;
    QLineEdit* m_inputLine;
    QWidget* m_inputContainer;
    QLabel* m_promptLabel;
    QWidget* m_headerBar;
    QPushButton* m_collapseButton;
    QPushButton* m_closeButton;
    bool m_isCollapsed;
    int m_originalHeight;
};
} // namespace openide::terminal
#endif
