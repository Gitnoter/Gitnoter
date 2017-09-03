#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include "notetablemodel.h"
#include "tagtablemodel.h"
#include "categoriestablemodel.h"

#include <QObject>
#include <QtCore>

class NoteModel : public QObject
{
public:
    NoteModel();
    NoteModel(QString noteText, QString filePath = "");
    NoteModel(NoteTableModel *noteTableModel, QList<TagTableModel *> *tagTableList,
              CategoriesTableModel *categoriesTableModel);

    QString getNote();

    NoteTableModel *noteTableModel;
    QList<TagTableModel *> *tagTableList;
    CategoriesTableModel *categoriesTableModel;

    void clear();

};

#endif // NOTEMODEL_H
