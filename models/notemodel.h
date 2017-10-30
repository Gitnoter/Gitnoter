#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include "tagmodel.h"
#include "categorymodel.h"

#include <QObject>
#include <QtCore>

class NoteModel : public QObject
{
Q_OBJECT

public:
    NoteModel(const QString noteText = "");

    NoteModel(const QString textPath, const QString dataPath = "");

    QString getDisplayNote() const;

    QString getTagsString() const;

    QString getNoteData() const;

    void setNoteData(const QString &noteData);

    void writerLocal();

    void clear();



    void setUuid(QString uuid = "");

    void setCreateDate(QString createDate = "");

    void setCreateDate(int createDate = 0);

    void setUpdateDate(QString updateDate = "");

    void setUpdateDate(int updateDate = 0);

    void setNoteText(QString body = "");

    QString getUuid();

    int getCreateDate();

    int getUpdateDate();

    QString getNoteText();

    const QString & getFileDir();

    const QStringList &getTagList() const;

    void setTagList(const QStringList &tagList = {});

    const QString &getCategory() const;

    void setCategory(const QString &category = "");

private:
    QString mUuid;
    int mCreateDate;
    int mUpdateDate;
    QString mBody;
    QStringList tagList;
    QString category;

};

#endif // NOTEMODEL_H
