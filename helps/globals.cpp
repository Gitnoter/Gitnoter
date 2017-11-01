#include "globals.h"
#include "tools.h"

#include <QStandardPaths>

const QString Globals::packageName = "com.huyaohui.gitnoter";
const QString Globals::version = "1.0.0";
const QString Globals::dateFormat = "yyyy/MM/dd hh:mm";
const QString Globals::tagSplit = ", ";

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

const QByteArray Globals::aesKey = "Q5tO3jUB0oe7FmWq";
const QByteArray Globals::aesIv  = "RNNSR8XNbMTuOSGd";

NoteModelList Globals::noteModelList;
CategoryModelList Globals::categoryModelList;
TagModelList Globals::tagModelList;

ConfigModel *Globals::configModel = new ConfigModel();
GitManager *Globals::gitManager = new GitManager();
ThemeManager *Globals::themeManager = new ThemeManager();

