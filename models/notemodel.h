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

    NoteModel(const QString textPath, const QString dataPath);

    QString toMarkdownHtml(int maxImageWidth = 980, bool forExport = false,
                           bool decrypt = true, bool base64Images = false);

    /**
     * Returns the CSS code for a QFont
     * Thank you to Phil Weinstein for the code
     */
    static QString encodeCssFont(const QFont& refFont);

    QString getStringFormTagList() const;

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

    QString getFileDir();

    const QStringList &getTagList() const;

    void setTagList(const QStringList &tagList = {});

    const QString &getCategory() const;

    void setCategory(const QString &category = "");

    const QString &getTitle() const;

    void setTitle(const QString &title);

private:
    QString mUuid;
    int mCreateDate;
    int mUpdateDate;
    QString mTitle;
    QString mNoteText;
    QStringList tagList;
    QString category;

};

#endif // NOTEMODEL_H
