#ifndef NOTEMODELLIST_H
#define NOTEMODELLIST_H

#include <QObject>

class NoteModel : public QObject
{
Q_OBJECT

public:
    /**
     * Returns the CSS code for a QFont
     * Thank you to Phil Weinstein for the code
     */
    static QString encodeCssFont(const QFont& refFont);

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

    QString getNoteDir();

    const QStringList &getTagList() const;

    void setTagList(const QStringList &tagList = {});

    const QString &getCategory() const;

    void setCategory(const QString &category = "");

    const QString &getTitle() const;

    void setTitle(const QString &title);

    int getIsDelete() const;

    void setIsDelete(int isDelete = 0);

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

class NoteModelList : public QObject
{
Q_OBJECT

public:
    NoteModelList();

    void init();

    NoteModel *findByUuid(const QString &uuid);

    void append(NoteModel *noteModel);

    NoteModel *append(const QString category);

    void prepend(NoteModel *noteModel);

    NoteModel *prepend(const QString category);

    void removeOne(NoteModel *noteModel);

    void deleteOne(NoteModel *noteModel);

    const QList<NoteModel *> &getList() const;

    void setList(const QList<NoteModel *> &noteModelList);

private:
    QList<NoteModel *> noteModelList;

};
#endif // NOTEMODELLIST_H
