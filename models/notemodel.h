#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include "contentmodel.h"
#include "tagsmodel.h"
#include "categoriesmodel.h"

#include <QObject>
#include <QtCore>

class NoteModel : public QObject
{
Q_OBJECT

public:
    ContentModel *contentModel;
    QList<TagsModel *> *tagsModelList;
    CategoriesModel *categoriesModel;

    NoteModel();

    NoteModel(CategoriesModel *categoriesModel);

    NoteModel(QString noteText, QString filePath = "");

    NoteModel(ContentModel *contentModel, QList<TagsModel *> *tagList,
              CategoriesModel *categoriesModel);

    QString getNote() const;

    QString getDisplayNote() const;

    QString getTagsString() const;

    void writerLocal();

    void clear();

};

#endif // NOTEMODEL_H
