#include "appconfig.h"
#include <QDir>

QString AppConfig::version = "1.0.0";
QString AppConfig::dateFormat = "yyyy/MM/dd hh:mm:ss";
QString AppConfig::tagSplit = ", ";
QString AppConfig::dbPath = "";
QString AppConfig::dbName = "";
QString AppConfig::repoPath = "";
QString AppConfig::noteFolderName = "notes";
QString AppConfig::audiosFolderName = "files/audios";
QString AppConfig::imagesFolderName = "files/images";
QString AppConfig::videosFolderName = "files/videos";

QString AppConfig::appConfigLocation = "";

void AppConfig::init(QString appConfigLocation) {
    AppConfig::appConfigLocation = appConfigLocation;

    dbPath = QDir(appConfigLocation).filePath("db/");
    dbName = "gitnoter.db";

    repoPath = QDir(appConfigLocation).filePath("user/user.git");
}
