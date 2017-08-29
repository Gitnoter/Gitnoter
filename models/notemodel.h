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
    NoteModel(QString noteText, QString filePath = "");
    NoteModel(NoteTableModel *noteTableModel, QList<TagTableModel *> *tagTableList,
              CategoriseTableModel *categoriesTableModel);

    QString getNote();

    NoteTableModel *noteTableModel;
    QList<TagTableModel *> *tagTableList;
    CategoriseTableModel *categoriesTableModel;

    void clear();

};

#endif // NOTEMODEL_H
