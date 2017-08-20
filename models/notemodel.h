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
    NoteModel(QString noteText);
    NoteModel(uint id, QString title, uint updateDate);
    NoteModel(uint id, QString title, QString categories, QStringList tags,
              uint createDate, uint updateDate, QString body);

    void setId(uint id);
    void setTitle(QString title);
    void setCreateDate(QString createDate);
    void setCreateDate(uint createDate);
    void setUpdateDate(QString updateDate);
    void setUpdateDate(uint updateDate);
    void setCategories(QString categories);
    void setTags(QList<QString> tags);
    void setTags(QString tags);
    void setBody(QString body);
    void setFileDir(QString fileDir);
    void setFileName(QString fileName);

    uint getId();
    QString getTitle();
    uint getCreateDate();
    uint getUpdateDate();
    QString getCategories();
    QStringList getTags();
    QString getBody();
    QString getNote();
    QString getFileDir();
    QString getFileName();

private:
    uint id;
    QString uuid;
    QString title;
    uint createDate;
    uint updateDate;
    QString categories;
    QStringList tags;
    QString body;
    QString fileDir;
    QString fileName;

    uint timestampFromDateTime(QString dateTime);
    QString timestampToDateTime(uint timestamp);
};

#endif // NOTEMODEL_H
