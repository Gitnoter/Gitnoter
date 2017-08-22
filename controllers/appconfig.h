#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QObject>

class AppConfig
{
public:
    static QString dateFormat;
    static QString tagSplit;
    static QString dbPath;
    static QString dbName;
    static QString repoPath;
    static QString noteFolderName;
    static QString audiosFolderName;
    static QString imagesFolderName;
    static QString videosFolderName;

    static QString appConfigLocation;

    // database -> config
    static QString version;
    static QString repoDir;
    static QString repoUrl;
    static QString repoUsername;
    static QString repoPassword;

    static void init(QString appConfigLocation);
    static void init(QString appConfigLocation, QString version, QString repoDir, QString repoUrl,
                     QString repoUsername, QString repoPassword);
    static void defaultInit();
};

#endif // APPCONFIG_H
