#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include "notetablemodel.h"
#include "tagtablemodel.h"
#include "categorisetablemodel.h"

#include <QObject>
#include <QtCore>

class NoteModel : public QObject
{
    Q_OBJECT
public:
    NoteModel(QString noteText);
    NoteModel(NoteTableModel *noteTableModel, QList<TagTableModel *> *tagTableList, CategoriseTableModel *categoriseTableModel);

    QString getNote();

public:
    NoteTableModel *noteTableModel;
    QList<TagTableModel *> *tagTableList;
    CategoriseTableModel *categoriseTableModel;

};

#endif // NOTEMODEL_H
