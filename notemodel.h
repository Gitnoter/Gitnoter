#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include <QObject>
#include <QtCore>

class NoteModel : public QObject
{
    Q_OBJECT
public:
    NoteModel(QString note);
    NoteModel(QString title, QString categories, QList<QString> tags,
              QString createDate, QString updateDate, QString body);

    void setTitle(QString title);
    void setCreateDate(QString createDate);
    void setUpdateDate(QString updateDate);
    void setCategories(QString categories);
    void setTags(QList<QString> tag);
    void setBody(QString body);

    QString getTitle();
    QString getCreateDate();
    QString getUpdateDate();
    QString getCategories();
    QStringList getTags();
    QString getBody();
    QString getNote();

private:
    QString title;
    QString createDate;
    QString updateDate;
    QString categories;
    QList<QString> tags;
    QString body;
};

#endif // NOTEMODEL_H
