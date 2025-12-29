#include "menu/NewFileDialog.hpp"
#include "MainWindow.hpp"
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QRegularExpression>

using namespace openide;
using namespace openide::menu;

NewFileDialog::NewFileDialog(MainWindow* parent, const QString& defaultDirectory)
    : QDialog(parent)
    , m_filePath("")
    , m_defaultDirectory(defaultDirectory)
    , m_directoryLineEdit(nullptr)
    , m_browseButton(nullptr)
    , m_fileNameLineEdit(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
{
    setupUI();
    
    setWindowTitle("New File");
    setModal(true);
    resize(500, 150);
    
    // Set default directory if provided - do this after setupUI so line edit exists
    if (!m_defaultDirectory.isEmpty()) {
        m_directoryLineEdit->setText(m_defaultDirectory);
        // Enable OK button if directory is set (user still needs to enter file name)
        m_okButton->setEnabled(false); // Still need file name
    }
}

void NewFileDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QFormLayout* formLayout = new QFormLayout();
    
    // Directory selection
    QHBoxLayout* directoryLayout = new QHBoxLayout();
    m_directoryLineEdit = new QLineEdit(this);
    m_directoryLineEdit->setReadOnly(true);
    m_directoryLineEdit->setPlaceholderText("Select a directory...");
    m_browseButton = new QPushButton("Browse...", this);
    directoryLayout->addWidget(m_directoryLineEdit);
    directoryLayout->addWidget(m_browseButton);
    formLayout->addRow("Location:", directoryLayout);
    
    // File name input
    m_fileNameLineEdit = new QLineEdit(this);
    m_fileNameLineEdit->setPlaceholderText("Enter file name (e.g., example.cpp)...");
    formLayout->addRow("File Name:", m_fileNameLineEdit);
    
    mainLayout->addLayout(formLayout);
    
    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_okButton = buttonBox->button(QDialogButtonBox::Ok);
    m_cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    
    mainLayout->addWidget(buttonBox);
    
    // Connect signals
    connect(m_browseButton, &QPushButton::clicked, this, &NewFileDialog::onBrowseClicked);
    connect(m_fileNameLineEdit, &QLineEdit::textChanged, this, &NewFileDialog::onFileNameChanged);
    connect(m_okButton, &QPushButton::clicked, this, &NewFileDialog::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &NewFileDialog::onCancelClicked);
    
    // Initially disable OK button until both directory and name are set
    m_okButton->setEnabled(false);
}

bool NewFileDialog::isValidFileName(const QString& fileName)
{
    if (fileName.isEmpty() || fileName.trimmed().isEmpty()) {
        return false;
    }
    // Check for invalid characters (Windows + Unix)
    QRegularExpression invalidChars("[<>:\"/\\|?*\\x00-\\x1F]");
    return !invalidChars.match(fileName).hasMatch();
}

bool NewFileDialog::createFile(const QString& filePath)
{
    QFile file(filePath);
    if (file.exists()) {
        return false; // File already exists
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false; // Failed to create
    }
    file.close();
    return true;
}

void NewFileDialog::onBrowseClicked()
{
    QString startDir = m_directoryLineEdit->text();
    if (startDir.isEmpty()) {
        startDir = m_defaultDirectory.isEmpty() ? QDir::homePath() : m_defaultDirectory;
    }
    
    QString selectedDir = QFileDialog::getExistingDirectory(
        this,
        "Select File Location",
        startDir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!selectedDir.isEmpty()) {
        m_directoryLineEdit->setText(selectedDir);
        // Update OK button state
        m_okButton->setEnabled(!selectedDir.isEmpty() && !m_fileNameLineEdit->text().isEmpty());
    }
}

void NewFileDialog::onFileNameChanged(const QString& text)
{
    // Update OK button state
    m_okButton->setEnabled(!m_directoryLineEdit->text().isEmpty() && !text.isEmpty());
}

void NewFileDialog::onOkClicked()
{
    QString directory = m_directoryLineEdit->text();
    QString fileName = m_fileNameLineEdit->text().trimmed();
    
    if (directory.isEmpty()) {
        QMessageBox::warning(this, "Invalid Location", "Please select a directory for the file.");
        return;
    }
    
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "Invalid File Name", "Please enter a file name.");
        return;
    }
    
    // Validate file name
    if (!isValidFileName(fileName)) {
        QMessageBox::warning(
            this,
            "Invalid File Name",
            "The file name contains invalid characters.\n"
            "Invalid characters: < > : \" / \\ | ? * and control characters"
        );
        return;
    }
    
    // Check if directory exists
    QDir dir(directory);
    if (!dir.exists()) {
        QMessageBox::warning(this, "Invalid Directory", "The selected directory does not exist.");
        return;
    }
    
    // Create full file path
    QString fullPath = dir.filePath(fileName);
    
    // Check if file already exists
    QFile file(fullPath);
    if (file.exists()) {
        QMessageBox::warning(
            this,
            "File Already Exists",
            QString("A file named '%1' already exists in the selected location.\nPlease choose a different name.")
                .arg(fileName)
        );
        return;
    }
    
    // Create the file
    if (!createFile(fullPath)) {
        QMessageBox::critical(
            this,
            "Error",
            QString("Failed to create file:\n%1").arg(fullPath)
        );
        return;
    }
    
    m_filePath = fullPath;
    accept();
}

void NewFileDialog::onCancelClicked()
{
    reject();
}

