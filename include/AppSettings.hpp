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
    QFont font() const;
    
    // Setters
    void setFontFamily(const QString& family);
    void setFontSize(int size);
    void setTabSpace(int spaces);
    
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
    
    void setDefaults();
};
}

#endif // APPSETTINGS_HPP



