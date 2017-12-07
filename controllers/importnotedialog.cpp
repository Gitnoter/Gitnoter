#include "ui_importprogressdialog.h"

#include "importnotedialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QCryptographicHash>
#include <QTemporaryFile>
#include <models/notemodel.h>

ImportNoteDialog::ImportNoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportProgressDialog)
{
    ui->setupUi(this);
}

ImportNoteDialog::~ImportNoteDialog()
{
    delete ui;
}

void ImportNoteDialog::init()
{
    QFileDialog *dialog = new QFileDialog();
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setNameFilter(tr("Enex files") + " (*.enex)");
    dialog->setWindowTitle(tr("Select Evernote enex file to import"));
    dialog->show();

    connect(dialog, &QFileDialog::fileSelected, [&](const QString &file) {
        importFile = file;
    });

    qDebug() << __func__;
}

void ImportNoteDialog::importNotes(QString data)
{
    QXmlQuery query;
    query.setFocus(data);
    query.setQuery("en-export/note");

    QXmlResultItems result;
    if (query.isValid()) {
        query.evaluateTo(&result);

        while (!result.next().isNull()) {
            query.setFocus(result.current());

            QString title;
            query.setQuery("title/text()");
            query.evaluateTo(&title);
            title = title.trimmed();

            QString content;
            query.setQuery("content/text()");

            // content seems to be html encoded
            query.evaluateTo(&content);

            content.replace("\\\"", "\"");

            // add a linebreak instead of div-containers
            content.replace(QRegularExpression("<\\/div>"), "\n");

            // convert html tags to markdown
            content = NoteModel::htmlToMarkdown(content);

            // import images
            content = importImages(content, query);

            // import attachments
            content = importAttachments(content, query);

            // remove all html tags
            content.remove(QRegularExpression("<.+?>"));

            // remove multiple \n
            content.replace(QRegularExpression("\n\n+"), "\n\n");
            content.replace(QRegularExpression("\n\n\\s+"), "\n\n");

#ifdef Q_OS_WIN32
            // removing or replacing some characters that are asking for
            // troubles
            title.replace("—", "-");
            title.replace("|", "-");
            title.replace("/", "-");
            title.replace("\\", "-");
            title.remove("#");
            title.remove("?");
            title.remove("'");
            title.remove("\"");
#endif

            const QString noteText = "#" + title + "\n\n" + content;

            NoteModel *noteModel = new NoteModel();
            noteModel->setNoteText(noteText);
            noteModel->saveNoteToLocal();

            ui->progressBar->setValue(ui->progressBar->value() + 1);
            QCoreApplication::processEvents();
        }

        if (result.hasError()) {
            qWarning() << "Error importing notes";
        }
    }
}

QString ImportNoteDialog::importImages(QString content, QXmlQuery query) {
    query.setQuery("resource");

    QXmlResultItems result;
    QHash<QString, MediaFileData> mediaFileDataHash;

    if (!query.isValid()) {
        return content;
    }

    query.evaluateTo(&result);
    QRegularExpressionMatch match;

    while (!result.next().isNull()) {
        QString objectType;
        QString suffix;
        query.setFocus(result.current());

        // parse the mime data of the object
        QString mime;
        query.setQuery("mime/text()");
        query.evaluateTo(&mime);

        QRegularExpression mimeExpression(
                "(.+)?\\/(.+)?", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch mimeMatch = mimeExpression.match(mime);

        if (mimeMatch.hasMatch()) {
            objectType = mimeMatch.captured(1);
            suffix = mimeMatch.captured(2);
        }

        // we only want to import images
        if (objectType != "image") {
            continue;
        }

        // check the recognition attribute for an object type
        QString recognition;
        query.setQuery("recognition/text()");
        query.evaluateTo(&recognition);

        recognition.replace("\\\"", "\"");

        match = QRegularExpression(
                "objID=\"(.+?)\"",
                QRegularExpression::CaseInsensitiveOption)
                .match(recognition);

        QString objectId;

        if (match.hasMatch()) {
            // get the object id of the resource to match it with the
            // en-media hash
            objectId = match.captured(1);
        }

        QString data;
        query.setQuery("data/text()");
        query.evaluateTo(&data);

        // if there is no object id set we generate the hash ourselves
        if (objectId.isEmpty()) {
            objectId = QString(QCryptographicHash::hash(
                    QByteArray::fromBase64(data.toLatin1()),
                    QCryptographicHash::Md5).toHex());
        }

        // store data in the QHash
        mediaFileDataHash[objectId].data = data;
        mediaFileDataHash[objectId].suffix = suffix;
    }

    if (mediaFileDataHash.count() == 0) {
        return content;
    }

    // match image tags
    QRegularExpression re("<en-media.+?type=\"image/.+?\".*?>",
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(content);
    QStringList importedObjectIds;

    // find all images
    while (i.hasNext()) {
        QRegularExpressionMatch imageMatch = i.next();
        QString imageTag = imageMatch.captured(0);

        // check for the hash
        QRegularExpression re2("hash=\"(.+?)\"",
                               QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch hashMatch = re2.match(imageTag);

        if (!hashMatch.hasMatch()) {
            continue;
        }

        QString objectId = hashMatch.captured(1);

        if (!mediaFileDataHash.contains(objectId)) {
            continue;
        }

        importedObjectIds << objectId;

        // get the base64 encoded image
        MediaFileData mediaFileData = mediaFileDataHash[objectId];

        // get the markdown code for the image file data entry
        QString markdownCode = getMarkdownForMediaFileData(mediaFileData);

        if (!markdownCode.isEmpty()) {
            // replace image tag with markdown code
            content.replace(imageTag, markdownCode);
        }
    }

    // get the image markdown code for missing images
    QHashIterator<QString, MediaFileData> hashIterator(mediaFileDataHash);
    while (hashIterator.hasNext()) {
        hashIterator.next();
        QString objectId = hashIterator.key();
        if (importedObjectIds.contains(objectId)) {
            continue;
        }

        MediaFileData mediaFileData = hashIterator.value();

        // get the markdown code for the image file data entry
        QString markdownCode = getMarkdownForMediaFileData(mediaFileData);

        content += "\n" + markdownCode;
    }

    return content;
}

QString ImportNoteDialog::importAttachments(QString content, QXmlQuery query) {
    query.setQuery("resource");

    QXmlResultItems result;
    QHash<QString, MediaFileData> mediaFileDataHash;

    if (!query.isValid()) {
        return content;
    }

    query.evaluateTo(&result);
    QRegularExpressionMatch match;

    while (!result.next().isNull()) {
        QString objectType;
        query.setFocus(result.current());

        // parse the mime data of the object
        QString mime;
        query.setQuery("mime/text()");
        query.evaluateTo(&mime);

        QRegularExpression mimeExpression(
                "(.+)?\\/(.+)?", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch mimeMatch = mimeExpression.match(mime);

        if (mimeMatch.hasMatch()) {
            objectType = mimeMatch.captured(1);
        }

        // we don't want to import images
        if (objectType == "image") {
            continue;
        }

        // check the recognition attribute for an object type
        QString recognition;
        query.setQuery("recognition/text()");
        query.evaluateTo(&recognition);

        recognition.replace("\\\"", "\"");

        match = QRegularExpression(
                "objID=\"(.+?)\"",
                QRegularExpression::CaseInsensitiveOption)
                .match(recognition);

        QString objectId;

        if (match.hasMatch()) {
            // get the object id of the resource to match it with the
            // en-media hash
            objectId = match.captured(1);
        }

        QString data;
        query.setQuery("data/text()");
        query.evaluateTo(&data);

        // if there is no object id set we generate the hash ourselves
        if (objectId.isEmpty()) {
            objectId = QString(QCryptographicHash::hash(
                    QByteArray::fromBase64(data.toLatin1()),
                    QCryptographicHash::Md5).toHex());
        }

        // find the filename
        QString fileName;
        query.setQuery("resource-attributes/file-name/text()");
        query.evaluateTo(&fileName);
        fileName = fileName.trimmed();

        // get the file suffix from the filename
        QFileInfo fileInfo(fileName);
        QString suffix = fileInfo.suffix();

        // store data in the QHash
        mediaFileDataHash[objectId].data = data;
        mediaFileDataHash[objectId].suffix = suffix;
        mediaFileDataHash[objectId].fileName = fileName;
    }

    if (mediaFileDataHash.count() == 0) {
        return content;
    }

    // match media tags
    QRegularExpression re("<en-media.+?type=\".+?\".*?>",
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(content);
    QStringList importedObjectIds;

    // find all files
    while (i.hasNext()) {
        QRegularExpressionMatch imageMatch = i.next();
        QString mediaTag = imageMatch.captured(0);

        // check for the hash
        QRegularExpression re2("hash=\"(.+?)\"",
                               QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch hashMatch = re2.match(mediaTag);

        if (!hashMatch.hasMatch()) {
            continue;
        }

        QString objectId = hashMatch.captured(1);

        if (!mediaFileDataHash.contains(objectId)) {
            continue;
        }

        importedObjectIds << objectId;

        // get the base64 encoded file
        MediaFileData mediaFileData = mediaFileDataHash[objectId];

        // get the markdown code for the file data entry
        QString markdownCode = getMarkdownForAttachmentFileData(mediaFileData);

        if (!markdownCode.isEmpty()) {
            // replace media tag with markdown code
            content.replace(mediaTag, markdownCode);
        }
    }

    // get the file markdown code for missing attachments
    QHashIterator<QString, MediaFileData> hashIterator(mediaFileDataHash);
    while (hashIterator.hasNext()) {
        hashIterator.next();
        QString objectId = hashIterator.key();
        if (importedObjectIds.contains(objectId)) {
            continue;
        }

        MediaFileData mediaFileData = hashIterator.value();

        // get the markdown code for the file data entry
        QString markdownCode = getMarkdownForAttachmentFileData(mediaFileData);

        content += "\n" + markdownCode;
    }

    return content;
}

QString ImportNoteDialog::getMarkdownForMediaFileData(MediaFileData &mediaFileData) {
    QString data = mediaFileData.data;
    QString imageSuffix = mediaFileData.suffix;

    // create a temporary file for the image
    QTemporaryFile *tempFile = new QTemporaryFile(
            QDir::tempPath() + QDir::separator() + "media-XXXXXX." +
            imageSuffix);

    if (!tempFile->open()) {
        return "";
    }

    // write image to the temporary file
    tempFile->write(QByteArray::fromBase64(data.toLatin1()));

    // store the temporary image in the media folder and return the
    // markdown code
    QString markdownCode = Note::getInsertMediaMarkdown(tempFile);
    return markdownCode;
}

/**
 * Returns the markdown code for an attachment file data entry
 *
 * @param mediaFileData
 * @return
 */
QString ImportNoteDialog::getMarkdownForAttachmentFileData(MediaFileData &mediaFileData) {
    QString data = mediaFileData.data;
    QString suffix = mediaFileData.suffix;
    QString fileName = mediaFileData.fileName;

    // create a temporary file for the attachment
    QTemporaryFile *tempFile = new QTemporaryFile(QDir::tempPath() + QDir::separator() + "media-XXXXXX." + suffix);

    if (!tempFile->open()) {
        return "";
    }

    // write file data to the temporary file
    tempFile->write(QByteArray::fromBase64(data.toLatin1()));

    // store the temporary file in the media folder and return the
    // markdown code
    QString markdownCode = Note::getInsertAttachmentMarkdown(tempFile,
                                                             fileName);
    return markdownCode;
}
