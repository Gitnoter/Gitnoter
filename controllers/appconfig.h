#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QObject>

class AppConfig : public QObject
{
public:
    static QString version;
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

    static void init(QString appConfigLocation);
};

#endif // APPCONFIG_H
