#include "globals.h"
#include "tools.h"

QString gPackageName = "com.huyaohui.gitnoter";
QString gVersion = "1.0.0";
QString gDateFormat = "yyyy/MM/dd hh:mm:ss";
QString gTagSplit = ", ";
QString gAppDataLocation = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath(gPackageName);
QString gDbPath = QDir(gAppDataLocation).filePath("db/");
QString gDbName = "gitnoter.db";
QString gRepoPath = QDir(gAppDataLocation).filePath("user/user.git");
QString gNoteFolderName = "notes";
QString gAudiosFolderName = "files/audios";
QString gImagesFolderName = "files/images";
QString gVideosFolderName = "files/videos";

QList<NoteModel *> gNoteModelList = {};
NoteModel *gOpenNoteModel = new NoteModel();
ConfigModel *gConfigModel = new ConfigModel();
GitManager *gGitManager = new GitManager();