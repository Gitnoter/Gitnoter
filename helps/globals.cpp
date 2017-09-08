#include "globals.h"

QString g_packageName = "com.huyaohui.gitnoter";
QString g_version = "1.0.0";
QString g_dateFormat = "yyyy/MM/dd hh:mm:ss";
QString g_tagSplit = ", ";
QString g_appDataLocation = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath(g_packageName);
QString g_dbPath = QDir(g_appDataLocation).filePath("db/");
QString g_dbName = "gitnoter.db";
QString g_repoPath = QDir(g_appDataLocation).filePath("user/user.git");
QString g_noteFolderName = "notes";
QString g_audiosFolderName = "files/audios";
QString g_imagesFolderName = "files/images";
QString g_videosFolderName = "files/videos";

Database *g_database = new Database();
NoteModel *g_noteModel = new NoteModel();
ConfigTableModel *g_configTableModel = new ConfigTableModel();
