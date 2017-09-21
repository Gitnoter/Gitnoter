#ifndef GLOBALS_H
#define GLOBALS_H

#include "notemodel.h"
#include "database.h"
#include "configtablemodel.h"
#include "tagstablemodel.h"
#include "gitmanager.h"

#include <QObject>

extern QString gPackageName;
extern QString gVersion;
extern QString gDateFormat;
extern QString gTagSplit;
extern QString gDbPath;
extern QString gDbName;
extern QString gRepoPath;
extern QString gNoteFolderName;
extern QString gAudiosFolderName;
extern QString gImagesFolderName;
extern QString gVideosFolderName;
extern QString gAppDataLocation;

extern Database *gDatabase;
extern NoteModel *gNoteModel;
extern ConfigTableModel *gConfigTableModel;
extern GitManager *gGitManager;


#endif // GLOBALS_H