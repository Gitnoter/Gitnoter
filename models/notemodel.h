#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include "contentmodel.h"
#include "tagsmodel.h"
#include "categoriesmodel.h"

#include <QObject>
#include <QtCore>

class NoteModel : public QObject
{
public:

    ContentModel *contentModel;
    QList<TagsModel *> *tagsModelList;
    CategoriesModel *categoriesModel;

    NoteModel();

    NoteModel(QString noteText, QString filePath = "");

    NoteModel(ContentModel *contentModel, QList<TagsModel *> *tagList,
              CategoriesModel *categoriesModel);

    QString getNote();

    void clear();

};

#endif // NOTEMODEL_H
