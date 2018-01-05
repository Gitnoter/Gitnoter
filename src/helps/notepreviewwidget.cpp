#include "notepreviewwidget.h"
#include "tools.h"
#include "globals.h"

#include <QTimer>
#include <QDebug>

NotePreviewWidget::NotePreviewWidget(QWidget *parent) : QTextBrowser(parent) {
}

void NotePreviewWidget::resizeEvent(QResizeEvent* event) {
    // we need this, otherwise the preview is always blank
    QTextBrowser::resizeEvent(event);

    if (urlImageList.length() != 0) {
        QTimer::singleShot(500, this, SLOT(changeImageWidth()));
    }

    emit resize(event->size(), event->oldSize());
}

QVariant NotePreviewWidget::loadResource(int type, const QUrl &name)
{
    if (QTextDocument::ImageResource == type) {
        const QString filePath = QDir(gTempPath).filePath("note-browser-" + Tools::md5(name.toString()) + ".jpg");
        UrlImage urlImage = getUrlImageByUrl(name.toString());

        if (urlImage.url.isEmpty()) {
            QPixmap pixmap;
            QByteArray data = Tools::readerFile(filePath);

            if (!data.isEmpty()) {
                pixmap.loadFromData(data);
            }
            else {
                data = Tools::downloadUrlToData(name);
                pixmap.loadFromData(data);
                Tools::writerFile(filePath, data);
            }

            urlImage.size = pixmap.size();
            urlImage.url = name.toString();
            urlImage.path = filePath;

            urlImageList.append(urlImage);
        }

        return QTextBrowser::loadResource(type, QUrl(urlImage.path));
    }
    return QTextBrowser::loadResource(type, name);
}

UrlImage NotePreviewWidget::getUrlImageByPath(const QString &path)
{
    for (auto &&item : urlImageList) {
        if (item.path == path) {
            return item;
        }
    }

    return UrlImage();
}

UrlImage NotePreviewWidget::getUrlImageByUrl(const QString &url)
{
    for (auto &&item : urlImageList) {
        if (item.url == url) {
            return item;
        }
    }

    return UrlImage();
}

void NotePreviewWidget::changeImageWidth()
{
    int widgetWidth = width() - 10;
    QString html = toHtml();
    QRegularExpression re("<img src=\"(http[s]+:\\/\\/[^\"]+)\"( width=\"[0-9]*\")?");
    QRegularExpressionMatchIterator i = re.globalMatch(html);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString replaceBefore =  match.captured(0);
        QString fileUrl = match.captured(1);
        UrlImage urlImage = getUrlImageByUrl(fileUrl);

        // for preview
        // cap the image width at maxImageWidth (note text view width)
        if (urlImage.path.isEmpty()) {
            continue;
        }

        int displayWidth = (urlImage.size.width() > widgetWidth) ? widgetWidth : urlImage.size.width();
        html.replace(replaceBefore, "<img src=\"" + fileUrl + "\" width=\"" + QString::number(displayWidth) + "\"");


        // encode the image base64
        //        if (base64Images) {
        //            QFile file(fileName);
        //
        //            if (!file.open(QIODevice::ReadOnly)) {
        //                qWarning() << QObject::tr("Could not read image file: %1")
        //                        .arg(fileName);
        //
        //                continue;
        //            }
        //
        //            QMimeDatabase db;
        //            QMimeType type = db.mimeTypeForFile(file.fileName());
        //            QByteArray ba = file.readAll();
        //
        //            html.replace(
        //                    QRegularExpression(
        //                            "<img(.+?)src=\"" + QRegularExpression::escape(
        //                                    fileUrl) + "\""),
        //                    QString("<img\\1src=\"data:%1;base64,%2\"").arg(
        //                            type.name(), QString(ba.toBase64())));
        //        }
    }
    setHtml(html);
}

void NotePreviewWidget::setHtml(const QString &text)
{
    urlImageList.clear();
    QTextEdit::setHtml(text);
}
