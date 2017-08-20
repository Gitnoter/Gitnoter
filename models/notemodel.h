#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include <QObject>
#include <QtCore>

#define __TAGS_SPLITE__ ", "

class NoteModel : public QObject
{
    Q_OBJECT
public:
    explicit NoteModel(QString noteText);
    NoteModel(QString id, QString title, int updateDate);
    NoteModel(QString id, QString title, QString categories, QStringList tags,
              int createDate, int updateDate, QString body);

    void setUuid(QString uuid);
    void setTitle(QString title);
    void setCreateDate(QString createDate);
    void setCreateDate(int createDate);
    void setUpdateDate(QString updateDate);
    void setUpdateDate(int updateDate);
    void setCategories(QString categories);
    void setTags(QStringList tags);
    void setTags(QString tags);
    void setBody(QString body);
    void setFileDir(QString fileDir);
    void setFileName(QString fileName);

    QString setUuid();
    QString getTitle();
    int getCreateDate();
    int getUpdateDate();
    QString getCategories();
    QStringList getTags();
    QString getBody();
    QString getNote();
    QString getFileDir();
    QString getFileName();

private:
    QString uuid;
    QString title;
    int createDate;
    int updateDate;
    QString categories;
    QStringList tags;
    QString body;
    QString fileDir;
    QString fileName;
};

#endif // NOTEMODEL_H
