#ifndef GLOBALS_H
#define GLOBALS_H

#include "notemodel.h"
#include "configmodel.h"
#include "tagsmodel.h"
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

extern QList<NoteModel *> gNoteModelList;
extern QList<CategoriesModel *> gCategoriesModelList;
extern QList<TagsModel *> gTagsModelList;
extern NoteModel *gOpenNoteModel;
extern ConfigModel *gConfigModel;
extern GitManager *gGitManager;

extern void gInitNoteModelList();

extern void gInitCategoriesModelList();

extern void gInitTagsModelList();

extern NoteModel *gGetNoteModelByUuid(const QString &uuid);

extern void gInitConfigModel();

#endif // GLOBALS_H