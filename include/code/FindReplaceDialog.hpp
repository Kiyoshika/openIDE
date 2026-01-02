#ifndef FINDREPLACEDIALOG_HPP
#define FINDREPLACEDIALOG_HPP

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>

// forward decl
namespace openide::code { class CodeEditor; }

namespace openide::code
{
class FindReplaceDialog : public QDialog
{
    Q_OBJECT
public:
    FindReplaceDialog(CodeEditor* editor, QWidget* parent = nullptr);
    ~FindReplaceDialog() = default;
    
private slots:
    void onFindNext();
    void onReplace();
    void onReplaceAll();
    
private:
    void setupUI();
    bool findText(const QString& text, bool forward = true);
    
    CodeEditor* m_editor;
    QLineEdit* m_findLineEdit;
    QLineEdit* m_replaceLineEdit;
    QPushButton* m_findButton;
    QPushButton* m_replaceButton;
    QPushButton* m_replaceAllButton;
    QPushButton* m_closeButton;
    QCheckBox* m_caseSensitiveCheckBox;
    QCheckBox* m_wholeWordsCheckBox;
    QLabel* m_statusLabel;
};
}

#endif // FINDREPLACEDIALOG_HPP



