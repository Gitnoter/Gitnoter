#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include <QObject>
#include <QtCore>

#define __TAGS_SPLITE__ ", "
#define __DATE_FORMAT__ "yyyy/MM/dd hh:mm:ss"

class NoteModel : public QObject
{
    Q_OBJECT
public:
    explicit NoteModel(QString noteText);
    NoteModel(int id, QString title, int updateDate);
    NoteModel(int id, QString title, QString categories, QStringList tags,
              int createDate, int updateDate, QString body);

    void setId(int id);
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

    int getId();
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
    int id;
    QString uuid;
    QString title;
    int createDate;
    int updateDate;
    QString categories;
    QStringList tags;
    QString body;
    QString fileDir;
    QString fileName;

    int timestampFromDateTime(QString dateTime);
    QString timestampToDateTime(int timestamp);
};

#endif // NOTEMODEL_H
