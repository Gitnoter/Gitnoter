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

public:
    NoteTableModel *noteTableModel;
    QList<TagTableModel *> *tagTableList;
    CategoriseTableModel *categoriesTableModel;

};

#endif // NOTEMODEL_H
