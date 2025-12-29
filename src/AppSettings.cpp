#include "AppSettings.hpp"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFontDatabase>
#include <QDebug>

using namespace openide;

AppSettings::AppSettings()
    : m_fontFamily("")
    , m_fontSize(12)
    , m_tabSpace(4)
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
        qDebug() << "Failed to open config file for reading:" << configPath;
        setDefaults();
        return false;
    }
    
    QTextStream in(&file);
    QString jsonContent = in.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonContent.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse config file:" << error.errorString();
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
    
    setDefaults(); // Ensure all values are valid
    return true;
}

bool AppSettings::saveToFile() const
{
    QString configPath = getConfigFilePath();
    QFile file(configPath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open config file for writing:" << configPath;
        return false;
    }
    
    QJsonObject obj;
    obj["fontFamily"] = m_fontFamily;
    obj["fontSize"] = m_fontSize;
    obj["tabSpace"] = m_tabSpace;
    
    QJsonDocument doc(obj);
    QTextStream out(&file);
    out << doc.toJson();
    file.close();
    
    return true;
}

