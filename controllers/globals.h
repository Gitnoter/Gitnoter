#ifndef APPDATA_H
#define APPDATA_H

#include "notemodel.h"
#include "database.h"

#include <QObject>

extern QString g_packageName;
extern QString g_version;
extern QString g_dateFormat;
extern QString g_tagSplit;
extern QString g_dbPath;
extern QString g_dbName;
extern QString g_repoPath;
extern QString g_noteFolderName;
extern QString g_audiosFolderName;
extern QString g_imagesFolderName;
extern QString g_videosFolderName;
extern QString g_appDataLocation;

extern Database *g_database;
extern NoteModel *g_noteModel;


#endif // APPDATA_H