#include "code/FindReplaceDialog.hpp"
#include "code/CodeEditor.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTextCursor>
#include <QTextDocument>

using namespace openide::code;

FindReplaceDialog::FindReplaceDialog(CodeEditor* editor, QWidget* parent)
    : QDialog(parent)
    , m_editor(editor)
    , m_findLineEdit(nullptr)
    , m_replaceLineEdit(nullptr)
    , m_findButton(nullptr)
    , m_replaceButton(nullptr)
    , m_replaceAllButton(nullptr)
    , m_closeButton(nullptr)
    , m_caseSensitiveCheckBox(nullptr)
    , m_wholeWordsCheckBox(nullptr)
    , m_statusLabel(nullptr)
{
    setupUI();
    setWindowTitle("Find and Replace");
    setModal(false);
    resize(400, 200);
}

void FindReplaceDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Find section
    QFormLayout* formLayout = new QFormLayout();
    m_findLineEdit = new QLineEdit(this);
    m_replaceLineEdit = new QLineEdit(this);
    
    formLayout->addRow("Find:", m_findLineEdit);
    formLayout->addRow("Replace:", m_replaceLineEdit);
    mainLayout->addLayout(formLayout);
    
    // Options
    QHBoxLayout* optionsLayout = new QHBoxLayout();
    m_caseSensitiveCheckBox = new QCheckBox("Case Sensitive", this);
    m_wholeWordsCheckBox = new QCheckBox("Whole Words", this);
    optionsLayout->addWidget(m_caseSensitiveCheckBox);
    optionsLayout->addWidget(m_wholeWordsCheckBox);
    optionsLayout->addStretch();
    mainLayout->addLayout(optionsLayout);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_findButton = new QPushButton("Find Next", this);
    m_replaceButton = new QPushButton("Replace", this);
    m_replaceAllButton = new QPushButton("Replace All", this);
    m_closeButton = new QPushButton("Close", this);
    
    buttonLayout->addWidget(m_findButton);
    buttonLayout->addWidget(m_replaceButton);
    buttonLayout->addWidget(m_replaceAllButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_closeButton);
    mainLayout->addLayout(buttonLayout);
    
    // Status label
    m_statusLabel = new QLabel(this);
    mainLayout->addWidget(m_statusLabel);
    
    // Connect signals
    connect(m_findButton, &QPushButton::clicked, this, &FindReplaceDialog::onFindNext);
    connect(m_replaceButton, &QPushButton::clicked, this, &FindReplaceDialog::onReplace);
    connect(m_replaceAllButton, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceAll);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(m_findLineEdit, &QLineEdit::returnPressed, this, &FindReplaceDialog::onFindNext);
}

bool FindReplaceDialog::findText(const QString& text, bool forward)
{
    if (!m_editor || text.isEmpty()) {
        m_statusLabel->setText("No text to find");
        return false;
    }
    
    QTextDocument::FindFlags flags;
    if (m_caseSensitiveCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (m_wholeWordsCheckBox->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }
    if (!forward) {
        flags |= QTextDocument::FindBackward;
    }
    
    bool found = m_editor->find(text, flags);
    
    if (found) {
        m_statusLabel->setText("");
        return true;
    } else {
        // Try wrapping around from the beginning
        QTextCursor cursor = m_editor->textCursor();
        cursor.movePosition(forward ? QTextCursor::Start : QTextCursor::End);
        m_editor->setTextCursor(cursor);
        
        found = m_editor->find(text, flags);
        if (found) {
            m_statusLabel->setText("Wrapped around");
            return true;
        } else {
            m_statusLabel->setText("Text not found");
            return false;
        }
    }
}

void FindReplaceDialog::onFindNext()
{
    QString searchText = m_findLineEdit->text();
    findText(searchText, true);
}

void FindReplaceDialog::onReplace()
{
    if (!m_editor) return;
    
    QString searchText = m_findLineEdit->text();
    QString replaceText = m_replaceLineEdit->text();
    
    if (searchText.isEmpty()) {
        m_statusLabel->setText("No text to find");
        return;
    }
    
    // Check if current selection matches the search text
    QTextCursor cursor = m_editor->textCursor();
    QString selectedText = cursor.selectedText();
    
    bool matches = false;
    if (m_caseSensitiveCheckBox->isChecked()) {
        matches = (selectedText == searchText);
    } else {
        matches = (selectedText.toLower() == searchText.toLower());
    }
    
    if (matches) {
        // Replace the current selection
        cursor.insertText(replaceText);
        m_statusLabel->setText("Replaced 1 occurrence");
    }
    
    // Find next occurrence
    findText(searchText, true);
}

void FindReplaceDialog::onReplaceAll()
{
    if (!m_editor) return;
    
    QString searchText = m_findLineEdit->text();
    QString replaceText = m_replaceLineEdit->text();
    
    if (searchText.isEmpty()) {
        m_statusLabel->setText("No text to find");
        return;
    }
    
    // Move to the beginning
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    m_editor->setTextCursor(cursor);
    
    int count = 0;
    
    QTextDocument::FindFlags flags;
    if (m_caseSensitiveCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (m_wholeWordsCheckBox->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }
    
    // Begin undo block for single undo operation
    cursor.beginEditBlock();
    
    while (m_editor->find(searchText, flags)) {
        QTextCursor foundCursor = m_editor->textCursor();
        foundCursor.insertText(replaceText);
        count++;
    }
    
    cursor.endEditBlock();
    
    if (count > 0) {
        m_statusLabel->setText(QString("Replaced %1 occurrence(s)").arg(count));
    } else {
        m_statusLabel->setText("Text not found");
    }
}


