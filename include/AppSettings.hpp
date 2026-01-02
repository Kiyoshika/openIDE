#ifndef APPSETTINGS_HPP
#define APPSETTINGS_HPP

#include <QString>
#include <QFont>
#include <QDir>

namespace openide
{
class AppSettings
{
public:
    AppSettings();
    
    // Getters
    QString fontFamily() const { return m_fontFamily; }
    int fontSize() const { return m_fontSize; }
    int tabSpace() const { return m_tabSpace; }
    QString projectTreeFontFamily() const { return m_projectTreeFontFamily; }
    int projectTreeFontSize() const { return m_projectTreeFontSize; }
    QString terminalFontFamily() const { return m_terminalFontFamily; }
    int terminalFontSize() const { return m_terminalFontSize; }
    QFont font() const;
    
    // Setters
    void setFontFamily(const QString& family);
    void setFontSize(int size);
    void setTabSpace(int spaces);
    void setProjectTreeFontFamily(const QString& family);
    void setProjectTreeFontSize(int size);
    void setTerminalFontFamily(const QString& family);
    void setTerminalFontSize(int size);
    
    // Config file operations
    bool loadFromFile();
    bool saveToFile() const;
    
    // Get config file path (cross-platform)
    static QString getConfigFilePath();
    static QString getConfigDirectory();
    
private:
    QString m_fontFamily;
    int m_fontSize;
    int m_tabSpace;
    QString m_projectTreeFontFamily;
    int m_projectTreeFontSize;
    QString m_terminalFontFamily;
    int m_terminalFontSize;
    
    void setDefaults();
};
}

#endif // APPSETTINGS_HPP



