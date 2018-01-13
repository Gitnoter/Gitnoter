#ifndef IMPORTNOTEDIALOG_H
#define IMPORTNOTEDIALOG_H

#include "notemodel.h"

#include <QDialog>
#include <QXmlQuery>
#include <QXmlResultItems>

namespace Ui {
class ImportNoteDialog;
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

protected:
    void showEvent(QShowEvent *showEvent) override;

private:
    void initProgressBar(QString data);
    void importNotes(QString data);

    QString importImages(NoteModel *noteModel, QString content, QXmlQuery query);
    QString importAttachments(NoteModel *noteModel, QString content, QXmlQuery query);

    QString getMarkdownForMediaFileData(NoteModel *noteModel, MediaFileData &mediaFileData);
    QString getMarkdownForAttachmentFileData(NoteModel *noteModel, MediaFileData &mediaFileData);

    QString decodeHtmlEntities(QString text);

private:
    Ui::ImportNoteDialog *ui;

};

#endif // IMPORTNOTEDIALOG_H
