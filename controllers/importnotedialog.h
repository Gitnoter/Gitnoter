#ifndef IMPORTNOTEDIALOG_H
#define IMPORTNOTEDIALOG_H

#include <QDialog>
#include <QXmlQuery>
#include <QXmlResultItems>

namespace Ui {
class ImportProgressDialog;
}

class ImportNoteDialog : public QDialog
{
    Q_OBJECT

public:
    struct MediaFileData {
        QString data;
        QString suffix;
        QString fileName;
    };

    explicit ImportNoteDialog(QWidget *parent = 0);
    ~ImportNoteDialog();

    void init();

private:
    void importNotes(QString data);

    /**
     * Imports and embeds images from an Evernote note
     *
     * @param content
     */
    QString importImages(QString content, QXmlQuery query);

    /**
     * Imports and embeds attachments from an Evernote note
     *
     * @param content
     */
    QString importAttachments(QString content, QXmlQuery query);

    /**
     * Returns the markdown code for an image file data entry
     *
     * @param mediaFileData
     * @return
     */
    QString getMarkdownForMediaFileData(MediaFileData &mediaFileData);

    /**
     * Returns the markdown code for an attachment file data entry
     *
     * @param mediaFileData
     * @return
     */
    QString getMarkdownForAttachmentFileData(MediaFileData &mediaFileData);

private:
    Ui::ImportProgressDialog *ui;
    QString importFile;

};

#endif // IMPORTNOTEDIALOG_H
