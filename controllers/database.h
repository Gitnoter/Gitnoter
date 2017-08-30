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

private:
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
    QList<TagTableModel *> selectTagsTable();
    TagTableModel* selectTagsTableById(int id);
    TagTableModel* selectTagsTableByName(QString name);

    int insertCategoriesTable(QString name);
    bool deleteCategoriesTableByName(QString name);
    bool updateCategoriesTableByName(QString name, QString whereName);
    QList<CategoriseTableModel *> selectCategoriesTable();
    CategoriseTableModel* selectCategoriesTableById(int id);
    CategoriseTableModel* selectCategoriesTableById(QString name);

    int insertNJTTable(QString notesUuid, int tagsId);
    bool deleteNJTTableByNotesUuid(QString notesUuid);
    bool deleteNJTTableById(QString notesUuid, int tagsId);
    QList<TagTableModel *>* selectNJTTableByNotesUuid(QString notesUuid);
    QList<TagTableModel *> selectNJTTableByTagsId(int tagsId);

    int insertNJCTable(QString notesUuid, int categoriesId);
    bool deleteNJCTableByNotesUuid(QString notesUuid);
    bool deleteNJCTableById(QString notesUuid, int categoriesId);
    QList<CategoriseTableModel *> selectNJCTableByNotesUuid(QString notesUuid);
    QList<CategoriseTableModel *> selectNJCTableByTagsId(int categoriesId);


private:
    QSqlDatabase db;
    QSqlQuery query;
};

#endif // DATABASE_H
