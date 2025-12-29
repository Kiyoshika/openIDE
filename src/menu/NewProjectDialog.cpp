#include "menu/NewProjectDialog.hpp"
#include "MainWindow.hpp"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QRegularExpression>

using namespace openide;
using namespace openide::menu;

NewProjectDialog::NewProjectDialog(MainWindow* parent)
    : QDialog(parent)
    , m_projectPath("")
    , m_projectName("")
    , m_directoryLineEdit(nullptr)
    , m_browseButton(nullptr)
    , m_projectNameLineEdit(nullptr)
    , m_previewLabel(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
{
    setupUI();
    
    setWindowTitle("New Project");
    setModal(true);
    resize(500, 200);
}

void NewProjectDialog::setupUI()
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
    
    // Project name input
    m_projectNameLineEdit = new QLineEdit(this);
    m_projectNameLineEdit->setPlaceholderText("Enter project name...");
    formLayout->addRow("Project Name:", m_projectNameLineEdit);
    
    // Preview label
    m_previewLabel = new QLabel(this);
    m_previewLabel->setText("Preview: ");
    m_previewLabel->setStyleSheet("color: gray; font-style: italic;");
    formLayout->addRow("", m_previewLabel);
    
    mainLayout->addLayout(formLayout);
    
    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_okButton = buttonBox->button(QDialogButtonBox::Ok);
    m_cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    
    mainLayout->addWidget(buttonBox);
    
    // Connect signals
    connect(m_browseButton, &QPushButton::clicked, this, &NewProjectDialog::onBrowseClicked);
    connect(m_projectNameLineEdit, &QLineEdit::textChanged, this, &NewProjectDialog::onProjectNameChanged);
    connect(m_okButton, &QPushButton::clicked, this, &NewProjectDialog::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &NewProjectDialog::onCancelClicked);
    
    // Initially disable OK button until both directory and name are set
    m_okButton->setEnabled(false);
}

QString NewProjectDialog::convertProjectName(const QString& input)
{
    QString result = input;
    // Remove tabs
    result.remove('\t');
    // Filter invalid directory characters
    result.remove(QRegularExpression("[<>:\"/\\|?*]"));
    // Convert to lowercase
    result = result.toLower();
    // Replace spaces with dashes
    result.replace(' ', '-');
    return result;
}

bool NewProjectDialog::validateProjectDirectory(const QString& basePath, const QString& projectName)
{
    if (basePath.isEmpty() || projectName.isEmpty()) {
        return false;
    }
    
    QDir dir(basePath);
    if (!dir.exists()) {
        return false;
    }
    
    QString fullPath = dir.filePath(projectName);
    return !QDir(fullPath).exists();
}

void NewProjectDialog::onBrowseClicked()
{
    QString selectedDir = QFileDialog::getExistingDirectory(
        this,
        "Select Project Location",
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!selectedDir.isEmpty()) {
        m_directoryLineEdit->setText(selectedDir);
        m_projectPath = selectedDir;
        // Update OK button state
        m_okButton->setEnabled(!m_projectPath.isEmpty() && !m_projectNameLineEdit->text().isEmpty());
    }
}

void NewProjectDialog::onProjectNameChanged(const QString& text)
{
    QString converted = convertProjectName(text);
    m_previewLabel->setText("Preview: " + (converted.isEmpty() ? "(empty)" : converted));
    
    // Update OK button state
    m_okButton->setEnabled(!m_projectPath.isEmpty() && !text.isEmpty());
}

void NewProjectDialog::onOkClicked()
{
    QString projectNameInput = m_projectNameLineEdit->text();
    if (projectNameInput.isEmpty()) {
        QMessageBox::warning(this, "Invalid Project Name", "Please enter a project name.");
        return;
    }
    
    if (m_projectPath.isEmpty()) {
        QMessageBox::warning(this, "Invalid Location", "Please select a project location.");
        return;
    }
    
    QString convertedName = convertProjectName(projectNameInput);
    if (convertedName.isEmpty()) {
        QMessageBox::warning(this, "Invalid Project Name", "The project name contains only invalid characters.");
        return;
    }
    
    // Check if directory already exists
    if (!validateProjectDirectory(m_projectPath, convertedName)) {
        QMessageBox::warning(
            this,
            "Directory Already Exists",
            QString("A directory named '%1' already exists in the selected location.\nPlease choose a different name.")
                .arg(convertedName)
        );
        return;
    }
    
    // Create the directory
    QDir baseDir(m_projectPath);
    QString fullPath = baseDir.filePath(convertedName);
    QDir dir;
    
    if (!dir.mkpath(fullPath)) {
        QMessageBox::critical(
            this,
            "Error",
            QString("Failed to create project directory:\n%1").arg(fullPath)
        );
        return;
    }
    
    m_projectName = convertedName;
    m_projectPath = fullPath;
    accept();
}

void NewProjectDialog::onCancelClicked()
{
    reject();
}

