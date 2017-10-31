#include "globals.h"
#include "tools.h"

#include <QStandardPaths>

const QString Global::packageName = "com.huyaohui.gitnoter";
const QString Global::version = "1.0.0";
const QString Global::dateFormat = "yyyy/MM/dd hh:mm";
const QString Global::tagSplit = ", ";

const QString Global::appDataLocation = QDir(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath(
        packageName);
const QString Global::appDataPath = QDir(appDataLocation).filePath("data/");
const QString Global::appConfigPath = QDir(appDataPath).filePath("appconfig.json");
const QString Global::repoPath = QDir(appDataLocation).filePath("user/user.git");
const QString Global::repoNoteTextPath = QDir(repoPath).filePath("notes/");
const QString Global::noteTextFileName = "markdown.md";
const QString Global::noteDataFileName = "data.json";
const QString Global::repoNoteDataPath = QDir(repoPath).filePath("data");
const QString Global::repoCategoryListPath = QDir(repoNoteDataPath).filePath("categories.list");
const QString Global::repoTagsListPath = QDir(repoNoteDataPath).filePath("tags.list");

const QString Global::newNoteKeySequence = "Ctrl+N";
const QString Global::lockWindowKeySequence = "Ctrl+Shift+L";
const QString Global::cutWindowKeySequence = "Ctrl+Shift+X";

const QByteArray Global::aesKey = "Q5tO3jUB0oe7FmWq";
const QByteArray Global::aesIv  = "RNNSR8XNbMTuOSGd";

NoteModelList Global::noteModelList;
CategoryModelList Global::categoryModelList;
TagModelList Global::tagModelList;

ConfigModel *Global::configModel = new ConfigModel();
GitManager *Global::gitManager = new GitManager();
ThemeManager *Global::themeManager = new ThemeManager();

