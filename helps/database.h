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

    void initTables();

    void addNoteText(NoteModel *noteModel);
    NoteModel* getNoteByUuid(QString uuid);
    void deleteNoteByUuid(QString uuid);
    QList<NoteTableModel *>* getSidebarNotes();

public:
    void connect(QString filename);
    void createTables();

    QString insertNotesTable(NoteTableModel *noteTableModel);
    bool deleteNotesTableByUuid(QString uuid);
    bool updateNotesTableByUuid(QString uuid, QString title, QString body, int updateDate);
    bool updateNotesTableByUuid(NoteTableModel *noteTableModel);
    NoteTableModel* selectNotesTableByUuid(QString uuid);

    int insertTagsTable(QString name);
    bool deleteTagsTableByName(QString name);
    bool updateTagsTableByName(QString name, QString whereName);
    QList<TagsTableModel *> selectTagsTable();
    TagsTableModel* selectTagsTableById(int id);
    TagsTableModel* selectTagsTableByName(QString name);

    int insertCategoriesTable(QString name);
    bool deleteCategoriesTableByName(QString name);
    bool updateCategoriesTableByName(QString name, QString whereName);
    QList<CategoriesTableModel *> selectCategoriesTable();
    CategoriesTableModel* selectCategoriesTableById(int id);
    CategoriesTableModel* selectCategoriesTableById(QString name);

    int insertNJTTable(QString notesUuid, int tagsId);
    bool deleteNJTTableByNotesUuid(QString notesUuid);
    bool deleteNJTTableById(QString notesUuid, int tagsId);
    QList<TagsTableModel *>* selectNJTTableByNotesUuid(QString notesUuid);
    QList<TagsTableModel *> selectNJTTableByTagsId(int tagsId);

    int insertNJCTable(QString notesUuid, int categoriesId);
    bool deleteNJCTableByNotesUuid(QString notesUuid);
    bool deleteNJCTableById(QString notesUuid, int categoriesId);
    QList<CategoriesTableModel *> selectNJCTableByNotesUuid(QString notesUuid);
    QList<CategoriesTableModel *> selectNJCTableByCategoriesId(int categoriesId);


private:
    QSqlDatabase mDb;
    QSqlQuery mQuery;
};

#endif // DATABASE_H
