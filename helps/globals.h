#ifndef GLOBALS_H
#define GLOBALS_H

#include "notemodel.h"
#include "configmodel.h"
#include "tagsmodel.h"
#include "gitmanager.h"

#include <QObject>

class Global : public QObject
{
//Q_OBJECT

public:
    static const QString packageName;
    static const QString version;
    static const QString dateFormat;
    static const QString tagSplit;

    static const QString appDataLocation;
    static const QString appDataPath;
    static const QString appConfigPath;
    static const QString repoPath;
    static const QString repoNotePath;
    static const QString repoResourcePath;
    static const QString repoDataPath;
    static const QString repoCategoriesListPath;
    static const QString repoTagsListPath;

    static QList<NoteModel *> noteModelList;
    static QList<CategoriesModel *> categoriesModelList;
    static QList<TagsModel *> tagsModelList;

    static NoteModel *openNoteModel;
    static ConfigModel *configModel;
    static GitManager *gitManager;

    static void initNoteModelList();

    static void initCategoriesModelList();

    static void initTagsModelList();

    static NoteModel *getNoteModelByUuid(const QString &uuid);

    static void initConfigModel();

};

#endif // GLOBALS_H