#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include "groupmodel.h"

#include <QObject>

class GroupModel;

class NoteModel : public QObject
{
Q_OBJECT

public:
    QString getUuid();

    int getCreateDate();

    int getUpdateDate();

    const QString getCreateDateString();

    const QString getUpdateDateString();

    QString getNoteText();

    QString getNoteDir();

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

    QString getMarkdownHtml(int maxImageWidth = 980, bool forExport = false,
                            bool decrypt = true, bool base64Images = false);

    QString getStringFormTagList() const;

    QString getNoteData() const;

    void setNoteData(const QString &noteData);

    void saveNoteToLocal();

    void saveNoteTextToLocal();

    void saveNoteDataToLocal();

    void clear();

private:
    /**
     * Returns the CSS code for a QFont
     * Thank you to Phil Weinstein for the code
     */
    static QString encodeCssFont(const QFont& refFont);

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
