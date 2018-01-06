#include "notepreviewwidget.h"
#include "tools.h"
#include "globals.h"

#include <QTimer>
#include <QDebug>

NotePreviewWidget::NotePreviewWidget(QWidget *parent) :
        QTextBrowser(parent),
        mResizeWindow(true),
        mDownloadImageTimer(new QTimer(this)),
        mDownloadThread(new QThread(this))
{
    connect(mDownloadImageTimer, SIGNAL(timeout()), this, SLOT(downloadWebImageTimer()));
    connect(mDownloadThread, SIGNAL(started()), this, SLOT(downloadThreadStarted()));
}

void NotePreviewWidget::resizeEvent(QResizeEvent* event) {
    // we need this, otherwise the preview is always blank
    QTextBrowser::resizeEvent(event);

    if (mResizeWindow && mUrlImageList.length() != 0) {
        mResizeWindow = false;
        QTimer::singleShot(500, this, SLOT(changeImageWidth()));
    }

    emit resize(event->size(), event->oldSize());
}

QVariant NotePreviewWidget::loadResource(int type, const QUrl &name)
{
    if (QTextDocument::ImageResource == type) {
        UrlImage urlImage = getUrlImageByUrl(name.toDisplayString());

        if (name.scheme() == "file") {
            if (urlImage.size.isEmpty()) {
                urlImage.url = name.toString();
                urlImage.path = urlImage.url;

                QPixmap pixmap;
                QByteArray data = Tools::readerFile(name.toString());

                if (!data.isEmpty()) {
                    pixmap.loadFromData(data);
                    urlImage.size = pixmap.size();
                }

                insertUrlImageToList(urlImage);
            }
        }
        else if (gUrlResourceScheme.indexOf(name.scheme()) != -1) {
            const QString filePath = QDir(gTempPath).filePath("note-browser-" + Tools::md5(name.toString()) + ".jpg");

            if (urlImage.size.isEmpty()) {
                urlImage.url = name.toString();
                urlImage.path = filePath;

                QPixmap pixmap;
                QByteArray data = Tools::readerFile(filePath);

                if (!data.isEmpty()) {
                    pixmap.loadFromData(data);
                    urlImage.size = pixmap.size();
                }
                else {
                    if (mDownloadImageTimer->isActive()){
                        mDownloadImageTimer->stop();
                    }
                    mDownloadImageTimer->start(1000);
                }

                insertUrlImageToList(urlImage);
            }

            if (!urlImage.size.isEmpty()) {
                return QTextBrowser::loadResource(type, QUrl(urlImage.path));
            }
        }
    }

    return QTextBrowser::loadResource(type, name);
}

UrlImage NotePreviewWidget::getUrlImageByPath(const QString &path)
{
    for (auto &&item : mUrlImageList) {
        if (item.path == path) {
            return item;
        }
    }

    return UrlImage();
}

UrlImage NotePreviewWidget::getUrlImageByUrl(const QString &url)
{
    for (auto &&item : mUrlImageList) {
        if (item.url == url) {
            return item;
        }
    }

    return UrlImage();
}

void NotePreviewWidget::insertUrlImageToList(UrlImage urlImage)
{
    for (auto &item : mUrlImageList) {
        if (item.url == urlImage.url) {
            item = urlImage;
            return;
        }
    }

    mUrlImageList.append(urlImage);
}

void NotePreviewWidget::changeImageWidth()
{
    int widgetWidth = width() - 10;
    QString html = toHtml();
    QRegularExpression re("<img src=\"([file|http|https|ftp]+:\\/\\/[^\"]+)\"( width=\"[0-9]*\")?");
    QRegularExpressionMatchIterator i = re.globalMatch(html);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString replaceBefore =  match.captured(0);
        QString fileUrl = match.captured(1);
        UrlImage urlImage = getUrlImageByUrl(QUrl(fileUrl).toDisplayString());

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

    mResizeWindow = true;
}

void NotePreviewWidget::downloadWebImageTimer()
{
    qDebug() << __func__ << mUrlImageList.length();
    
    bool hasDownload = false;
    for (auto &&item : mUrlImageList) {
        if (item.size.isEmpty()) {
            hasDownload = true;
            break;
        }
    }

    if (!hasDownload) {
        mDownloadImageTimer->stop();
        mDownloadThread->exit();
        return;
    }

    if (!mDownloadThread->isRunning()) {
        mDownloadThread->start();
    }
}

void NotePreviewWidget::downloadThreadStarted()
{
    for (auto &&item : mUrlImageList) {
        if (!item.size.isEmpty()) {
            continue;
        }

        QByteArray data = Tools::downloadUrlToData(item.url);
        QPixmap pixmap;
        pixmap.loadFromData(data);
        Tools::writerFile(item.path, data);

        item.size = pixmap.size();
    }

    changeImageWidth();
}
