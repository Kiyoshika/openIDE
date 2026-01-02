#include "AppSettings.hpp"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFontDatabase>

using namespace openide;

AppSettings::AppSettings()
    : m_fontFamily("")
    , m_fontSize(12)
    , m_tabSpace(4)
    , m_projectTreeFontFamily("")
    , m_projectTreeFontSize(10)
    , m_terminalFontFamily("")
    , m_terminalFontSize(10)
{
    setDefaults();
}

void AppSettings::setDefaults()
{
    if (m_fontFamily.isEmpty()) {
        QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        m_fontFamily = font.family();
    }
    if (m_fontSize <= 0) {
        m_fontSize = 12;
    }
    if (m_tabSpace <= 0) {
        m_tabSpace = 4;
    }
    if (m_projectTreeFontFamily.isEmpty()) {
        QFont font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        m_projectTreeFontFamily = font.family();
    }
    if (m_projectTreeFontSize <= 0) {
        m_projectTreeFontSize = 10;
    }
    if (m_terminalFontFamily.isEmpty()) {
        QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        m_terminalFontFamily = font.family();
    }
    if (m_terminalFontSize <= 0) {
        m_terminalFontSize = 10;
    }
}

QFont AppSettings::font() const
{
    QFont font(m_fontFamily);
    font.setPointSize(m_fontSize);
    return font;
}

void AppSettings::setFontFamily(const QString& family)
{
    m_fontFamily = family;
    setDefaults(); // Ensure valid font
}

void AppSettings::setFontSize(int size)
{
    m_fontSize = size;
    setDefaults(); // Ensure valid size
}

void AppSettings::setTabSpace(int spaces)
{
    m_tabSpace = spaces;
    setDefaults(); // Ensure valid tab space
}

void AppSettings::setProjectTreeFontFamily(const QString& family)
{
    m_projectTreeFontFamily = family;
    setDefaults(); // Ensure valid font
}

void AppSettings::setProjectTreeFontSize(int size)
{
    m_projectTreeFontSize = size;
    setDefaults(); // Ensure valid size
}

void AppSettings::setTerminalFontFamily(const QString& family)
{
    m_terminalFontFamily = family;
    setDefaults(); // Ensure valid font
}

void AppSettings::setTerminalFontSize(int size)
{
    m_terminalFontSize = size;
    setDefaults(); // Ensure valid size
}

QString AppSettings::getConfigDirectory()
{
    QString homeDir = QDir::homePath();
    QString configDir = QDir(homeDir).filePath("openide");
    
    // Create directory if it doesn't exist
    QDir dir;
    if (!dir.exists(configDir)) {
        dir.mkpath(configDir);
    }
    
    return configDir;
}

QString AppSettings::getConfigFilePath()
{
    return QDir(getConfigDirectory()).filePath(".config");
}

bool AppSettings::loadFromFile()
{
    QString configPath = getConfigFilePath();
    QFile file(configPath);
    
    if (!file.exists()) {
        // File doesn't exist, use defaults
        setDefaults();
        return true;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setDefaults();
        return false;
    }
    
    QTextStream in(&file);
    QString jsonContent = in.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonContent.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        setDefaults();
        return false;
    }
    
    QJsonObject obj = doc.object();
    
    if (obj.contains("fontFamily") && obj["fontFamily"].isString()) {
        m_fontFamily = obj["fontFamily"].toString();
    }
    
    if (obj.contains("fontSize") && obj["fontSize"].isDouble()) {
        m_fontSize = obj["fontSize"].toInt();
    }
    
    if (obj.contains("tabSpace") && obj["tabSpace"].isDouble()) {
        m_tabSpace = obj["tabSpace"].toInt();
    }
    
    if (obj.contains("projectTreeFontFamily") && obj["projectTreeFontFamily"].isString()) {
        m_projectTreeFontFamily = obj["projectTreeFontFamily"].toString();
    }
    
    if (obj.contains("projectTreeFontSize") && obj["projectTreeFontSize"].isDouble()) {
        m_projectTreeFontSize = obj["projectTreeFontSize"].toInt();
    }
    
    if (obj.contains("terminalFontFamily") && obj["terminalFontFamily"].isString()) {
        m_terminalFontFamily = obj["terminalFontFamily"].toString();
    }
    
    if (obj.contains("terminalFontSize") && obj["terminalFontSize"].isDouble()) {
        m_terminalFontSize = obj["terminalFontSize"].toInt();
    }
    
    setDefaults(); // Ensure all values are valid
    return true;
}

bool AppSettings::saveToFile() const
{
    QString configPath = getConfigFilePath();
    QFile file(configPath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QJsonObject obj;
    obj["fontFamily"] = m_fontFamily;
    obj["fontSize"] = m_fontSize;
    obj["tabSpace"] = m_tabSpace;
    obj["projectTreeFontFamily"] = m_projectTreeFontFamily;
    obj["projectTreeFontSize"] = m_projectTreeFontSize;
    obj["terminalFontFamily"] = m_terminalFontFamily;
    obj["terminalFontSize"] = m_terminalFontSize;
    
    QJsonDocument doc(obj);
    QTextStream out(&file);
    out << doc.toJson();
    file.close();
    
    return true;
}

