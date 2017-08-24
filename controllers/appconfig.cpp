#include "appconfig.h"
#include <QDir>

QString AppConfig::dateFormat = "";
QString AppConfig::tagSplit = "";
QString AppConfig::version = "";
QString AppConfig::dbPath = "";
QString AppConfig::dbName = "";
QString AppConfig::repoPath = "";
QString AppConfig::noteFolderName = "";
QString AppConfig::audiosFolderName = "";
QString AppConfig::imagesFolderName = "";
QString AppConfig::videosFolderName = "";

QString AppConfig::appConfigLocation = "";

// database -> config
QString AppConfig::repoDir = "";
QString AppConfig::repoUrl = "";
QString AppConfig::repoUsername = "";
QString AppConfig::repoPassword = "";

void AppConfig::init(QString appConfigLocation) {
    AppConfig::appConfigLocation = appConfigLocation;
    AppConfig::defaultInit();
}

void AppConfig::init(QString appConfigLocation, QString version, QString repoDir, QString repoUrl, QString repoUsername,
                     QString repoPassword) {
    AppConfig::appConfigLocation = appConfigLocation;
    AppConfig::defaultInit();

    // database -> config
    AppConfig::repoDir = repoDir;
    AppConfig::repoUrl = repoUrl;
    AppConfig::repoUsername = repoUsername;
    AppConfig::repoPassword = repoPassword;
}

void AppConfig::defaultInit() {
    dateFormat = "yyyy/MM/dd hh:mm:ss";
    tagSplit = ", ";
    version = "1.0.0";

    dbPath = QDir(appConfigLocation).filePath("db/");
    dbName = "gitnoter.db";

    repoPath = QDir(appConfigLocation).filePath("user/user.git");
    noteFolderName = "notes";
    audiosFolderName = "files/audios";
    imagesFolderName = "files/images";
    videosFolderName = "files/videos";

}

