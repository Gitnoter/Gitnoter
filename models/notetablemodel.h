#ifndef NOTETABLEMODEL_H
#define NOTETABLEMODEL_H

#include <QObject>

class NoteTableModel : public QObject
{
    Q_OBJECT
public:
    explicit NoteTableModel(QString uuid, QString title, int insertDate);
    NoteTableModel(QString uuid, QString title, int insertDate, int updateDate, QString body);
    NoteTableModel(QString uuid, QString title, int insertDate, int updateDate, QString body, QString fileDir, QString fileName);

    void setUuid(QString uuid);
    void setTitle(QString title);
    void setCreateDate(QString createDate);
    void setCreateDate(int createDate);
    void setUpdateDate(QString updateDate);
    void setUpdateDate(int updateDate);
    void setBody(QString body);
    void setFileDir(QString fileDir);
    void setFileName(QString fileName);

    QString setUuid();
    QString getTitle();
    int getCreateDate();
    int getUpdateDate();
    QString getBody();
    QString getFileDir();
    QString getFileName();

private:
    QString uuid;
    QString title;
    int insertDate;
    int updateDate;
    QString body;
    QString fileDir;
    QString fileName;
};

#endif // NOTETABLEMODEL_H