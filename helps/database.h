#ifndef DATABASE_H
#define DATABASE_H

#include "notemodel.h"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class Database : public QObject
{
public:
    Database();
    Database(QString filename);
    ~Database();

    void inits();

    void addNoteText(NoteModel *noteModel);
    NoteModel* getNoteByUuid(QString uuid);
    void deleteNoteByUuid(QString uuid);
    QList<ContentModel *>* getSidebarNotes();

public:
    void connect(QString filename);
    void creates();

    QString insertNotes(ContentModel *contentModel);
    bool deleteNotesByUuid(QString uuid);
    bool updateNotesByUuid(QString uuid, QString title, QString body, int updateDate);
    bool updateNotesByUuid(ContentModel *contentModel);
    ContentModel* selectNotesByUuid(QString uuid);

    int insertTags(QString name);
    bool deleteTagsByName(QString name);
    bool updateTagsByName(QString name, QString whereName);
    QList<TagsModel *> selectTags();
    TagsModel* selectTagsById(int id);
    TagsModel* selectTagsByName(QString name);

    int insertCategories(QString name);
    bool deleteCategoriesByName(QString name);
    bool updateCategoriesByName(QString name, QString whereName);
    QList<CategoriesModel *> selectCategories();
    CategoriesModel* selectCategoriesById(int id);
    CategoriesModel* selectCategoriesById(QString name);

    int insertNJT(QString notesUuid, int tagsId);
    bool deleteNJTByNotesUuid(QString notesUuid);
    bool deleteNJTById(QString notesUuid, int tagsId);
    QList<TagsModel *>* selectNJTByNotesUuid(QString notesUuid);
    QList<TagsModel *> selectNJTByTagsId(int tagsId);

    int insertNJC(QString notesUuid, int categoriesId);
    bool deleteNJCByNotesUuid(QString notesUuid);
    bool deleteNJCById(QString notesUuid, int categoriesId);
    QList<CategoriesModel *> selectNJCByNotesUuid(QString notesUuid);
    QList<CategoriesModel *> selectNJCByCategoriesId(int categoriesId);


private:
    QSqlDatabase mDb;
    QSqlQuery mQuery;
};

#endif // DATABASE_H
