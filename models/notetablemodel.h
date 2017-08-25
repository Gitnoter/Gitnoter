#ifndef NOTETABLEMODEL_H
#define NOTETABLEMODEL_H

#include <QObject>

class NoteTableModel : public QObject
{
public:
    NoteTableModel(QString uuid, QString title, int createDate, int updateDate, QString body = "");

    void setUuid(QString uuid);
    void setTitle(QString title);
    void setCreateDate(QString createDate);
    void setCreateDate(int createDate);
    void setUpdateDate(QString updateDate);
    void setUpdateDate(int updateDate);
    void setBody(QString body);

    QString getUuid();
    QString getTitle();
    int getCreateDate();
    int getUpdateDate();
    QString getBody();

private:
    QString uuid;
    QString title;
    int createDate;
    int updateDate;
    QString body;
};

#endif // NOTETABLEMODEL_H