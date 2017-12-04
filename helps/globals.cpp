#include "globals.h"
#include "tools.h"

#include <QStandardPaths>

const QString Globals::packageName = "com.huyaohui.gitnoter";
const QString Globals::version = "1.0.0";
const QString Globals::dateFormat = "yyyy/MM/dd hh:mm";
const QString Globals::tagSplit = ", ";
const int Globals::sevenDays = 7 * 24 * 60 * 60;

const QString Globals::appDataLocation = QDir(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath(
        packageName);
const QString Globals::appDataPath = QDir(appDataLocation).filePath("data/");
const QString Globals::appConfigPath = QDir(appDataPath).filePath("appconfig.json");
const QString Globals::repoPath = QDir(appDataLocation).filePath("user/user.git");
const QString Globals::repoNoteTextPath = QDir(repoPath).filePath("notes/");
const QString Globals::noteTextFileName = "markdown.md";
const QString Globals::noteDataFileName = "data.json";
const QString Globals::repoNoteDataPath = QDir(repoPath).filePath("data");
const QString Globals::repoCategoryListPath = QDir(repoNoteDataPath).filePath("categories.list");
const QString Globals::repoTagListPath = QDir(repoNoteDataPath).filePath("tags.list");

const QString Globals::newNoteKeySequence = "Ctrl+N";
const QString Globals::lockWindowKeySequence = "Ctrl+Shift+L";
const QString Globals::cutWindowKeySequence = "Ctrl+Shift+X";

const int Globals::editorFontSize = 14;

//const QStringList Globals::searchEngines = {"https://www.baidu.com/s?wd=", "https://www.google.com/search?&q="};
const QString Globals::searchEngine = QLocale::system().name() == "zh_CN" ? "https://www.baidu.com/s?wd="
                                                                          : "https://www.google.com/search?&q=";

const QByteArray Globals::aesKey = "Q5tO3jUB0oe7FmWq";
const QByteArray Globals::aesIv  = "RNNSR8XNbMTuOSGd";

ConfigModel *Globals::configModel = new ConfigModel();
GitManager *Globals::gitManager = new GitManager();
ThemeManager *Globals::themeManager = new ThemeManager();

