#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include "groupmodel.h"

#include <QObject>
#include <QListWidget>

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


public:
    /**
     * Returns the CSS code for a QFont
     * Thank you to Phil Weinstein for the code
     */
    static QString encodeCssFont(const QFont& refFont);

    static QList<NoteModel *> init(QListWidget *listWidget);

    static void appendNew(QListWidget *listWidget, NoteModel *noteModel);

    static NoteModel *appendNew(QListWidget *listWidget, const QString &category);

    static void removeOne(QListWidget *listWidget, NoteModel *noteModel);

    static void deleteOne(QListWidget *listWidget, const QString &uuid);

    static void appendOld(QListWidget *listWidget, const QString &uuid);

    static QListWidgetItem *getItemByUuid(QListWidget *listWidget, const QString &uuid);

    static NoteModel *getNoteModelByUuid(QListWidget *listWidget, const QString &uuid);

    static int showListItems(QListWidget *listWidget, Gitnoter::GroupType type, const QString &name);

    static const QString setItemSelected(QListWidget *listWidget, const QString &uuid);

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