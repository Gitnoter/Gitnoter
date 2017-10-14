#ifndef GLOBALS_H
#define GLOBALS_H

#include "notemodel.h"
#include "configmodel.h"
#include "tagsmodel.h"
#include "gitmanager.h"

#include <QObject>

class Global : public QObject
{
Q_OBJECT

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
    static const QString repoResourceName;
    static const QString repoResourcePath;
    static const QString repoDataPath;
    static const QString repoCategoriesListPath;
    static const QString repoTagsListPath;

    static const QString newNoteKeySequence;
    static const QString lockWindowKeySequence;
    static const QString cutWindowKeySequence;

    static const QByteArray aesKey;
    static const QByteArray aesIv;

    static QList<NoteModel *> noteModelList;
    static QList<CategoriesModel *> categoriesModelList;
    static QList<TagsModel *> tagsModelList;

    static NoteModel *openNoteModel;
    static ConfigModel *configModel;
    static GitManager *gitManager;

    /// static QList<NoteModel *> noteModelList;

    static void initNoteModelList();

    static NoteModel *getNoteModelByUuid(const QString &uuid);

    static void addNoteModelToList(NoteModel *noteModel);

    /// static QList<CategoriesModel *> categoriesModelList;

    static void initCategoriesModelList();

    static bool addCategoriesModelList(QList<CategoriesModel *>);

    static bool hasInCategoriesModelList(const QString &name);

    static void saveCategoriesModelList();

    static void renameCategoriesModelListInName(const QString oldName, const QString newName);

    /// static QList<TagsModel *> tagsModelList;

    static void initTagsModelList();

    static bool addTagsModelList(QList<TagsModel *> list);

    static bool hasInTagsModelList(const QString &name);

    static void saveTagsModelList();

    static void renameTagsModelListInName(const QString oldName, const QString newName);

    /// static NoteModel *openNoteModel;

    static void setOpenNoteModel(NoteModel *noteModel);

    /// ConfigModel

    static void initConfigModel();

    ///

    static int initGitManager();

    static void initRepoLocalDir();

};

#endif // GLOBALS_H