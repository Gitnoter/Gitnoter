#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include "groupmodel.h"

#include <QObject>
#include <QUrl>
#include <QFile>

class GroupModel;

class NoteModel : public QObject
{
Q_OBJECT

public:
    QString getUuid();

    QString getShortUuid() const;

    int getCreateDate();

    int getUpdateDate();

    const QString getCreateDateString();

    const QString getUpdateDateString();

    QString getNoteText();

    QString getNoteDir();

    QString getFilePath(QString noteFilePath);

    const QStringList &getTagList() const;

    void setTagList(const QStringList &tagList = {});

    const QString &getCategory() const;

    void setCategory(const QString &category = "");

    const QString &getTitle() const;

    void setTitle(const QString &title);

    int getIsDelete() const;

    void setIsDelete(int isDelete = 0);

    void setUuid(QString uuid = "");

    void setCreateDate(QString createDate = "");

    void setCreateDate(int createDate = 0);

    void setUpdateDate(QString updateDate = "");

    void setUpdateDate(int updateDate = 0);

    void setNoteText(QString body = "");

public:
    NoteModel(const QString noteText = "");

    NoteModel(const QString textPath, const QString dataPath);

    QString getMarkdownHtml();

    QString getStringFormTagList() const;

    QString getNoteData() const;

    void setNoteData(const QString &noteData);

    void saveNoteToLocal();

    void saveNoteTextToLocal();

    void saveNoteDataToLocal();

    void clear();

public:
    /**
     * Returns the CSS code for a QFont
     * Thank you to Phil Weinstein for the code
     */
    static QString encodeCssFont(const QFont& refFont);

    static QString htmlToMarkdown(QString text);

    static QString downloadUrlToMedia(QString url, NoteModel *noteModel, bool returnUrlOnly = false);

    static QString byteArrayToMedia(const QByteArray &byteArray, NoteModel *noteModel, bool returnUrlOnly = false);

private:
    QString mUuid;
    int mCreateDate;
    int mUpdateDate;
    QString mTitle;
    QString mNoteText;
    QStringList tagList;
    QString category;
    int isDelete;

};
#endif // NOTEMODEL_H
