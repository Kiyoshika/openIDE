#ifndef TERMINALFRONTEND_HPP
#define TERMINALFRONTEND_HPP

#include <QAbstractScrollArea>
#include <QString>
#include <QPaintEvent>
#include <QShowEvent>
#include <QLineEdit>
#include <QLabel>

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
    
    TerminalBackendInterface* m_backend;
    QString m_output;
    QString m_currentDirectory;
    QLineEdit* m_inputLine;
    QWidget* m_inputContainer;
    QLabel* m_promptLabel;
};
} // namespace openide::terminal
#endif
