#ifndef GLOBALS_H
#define GLOBALS_H

#include "notemodel.h"
#include "configmodel.h"
#include "tagmodel.h"
#include "gitmanager.h"
#include "thememanager.h"

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
    static const QString repoNoteTextPath;
    static const QString noteTextFileName;
    static const QString noteDataFileName;
    static const QString repoNoteDataPath;
    static const QString repoCategoriesListPath;
    static const QString repoTagsListPath;

    static const QString newNoteKeySequence;
    static const QString lockWindowKeySequence;
    static const QString cutWindowKeySequence;

    static const QByteArray aesKey;
    static const QByteArray aesIv;

    static QList<NoteModel *> noteModelList;
    static QList<CategoryModel *> categoryModelList;
    static QList<TagModel *> tagModelList;

    static NoteModel *openNoteModel;
    static ConfigModel *configModel;
    static GitManager *gitManager;
    static ThemeManager *themeManager;

    /// static QList<NoteModel *> noteModelList;

    static void initNoteModelList();

    static NoteModel *getNoteModelByUuid(const QString &uuid);

    static void addNoteModelToList(NoteModel *noteModel);

    /// static QList<CategoryModel *> categoryModelList;

    static void initCategoryModelList();

    static void addCategoryModelToList(const QString categoryName);

    static bool hasInCategoryModelList(const QString &name);

    static void saveCategoryModelList();

    static void renameCategoryModelListInName(const QString oldName, const QString newName);

    /// static QList<TagModel *> tagsModelList;

    static void initTagModelList();

    static void addTagModelList(const QString tagName);

    static bool hasInTagModelList(const QString &name);

    static void saveTagModelList();

    static void renameTagModelListInName(const QString oldName, const QString newName);

    /// static NoteModel *openNoteModel;

    static void setOpenNoteModel(NoteModel *noteModel);

    /// ConfigModel

    static void initConfigModel();

    ///

    static int initGitManager();

    static void initRepoLocalDir();

};

#endif // GLOBALS_H
