#include "notepreviewwidget.h"
#include "tools.h"
#include "globals.h"

#include <QTimer>
#include <QDebug>

NotePreviewWidget::NotePreviewWidget(QWidget *parent) :
        QTextBrowser(parent),
        mSingleTimeout(new SingleTimeout(Gitnoter::ResetTimeout, this)),
        mDownloadThread(new QThread(this))
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(mDownloadThread, SIGNAL(started()), this, SLOT(downloadThreadStarted()));
}

void NotePreviewWidget::resizeEvent(QResizeEvent* event) {
    // we need this, otherwise the preview is always blank
    QTextBrowser::resizeEvent(event);
    qDebug() << event->size() << event->oldSize();
    if (mUrlImageList.length() != 0 && event->size().width() != event->oldSize().width()) {
        mSingleTimeout->singleShot(1000, this, SLOT(changeImageWidth()));
    }
}

QVariant NotePreviewWidget::loadResource(int type, const QUrl &name)
{
    if (QTextDocument::ImageResource == type) {
        UrlImage urlImage = getUrlImageByUrl(name.toDisplayString());
        return QTextBrowser::loadResource(type, QUrl(urlImage.path));
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
            if (item.size.isEmpty()) {
                return;
            }
            item = urlImage;
            return;
        }
    }

    mUrlImageList.append(urlImage);
}

void NotePreviewWidget::insertUrlImageToList(const QString &url, QString path)
{
    if (path.isEmpty()) {
        if (QUrl(url).scheme() == "file") {
            path = url;
        }
        else {
            path = "file://";
#ifdef Q_OS_WIN
            path += "/";
#endif
            path += QDir(gTempPath).filePath("note-browser-" + Tools::md5(url) + ".jpg");
        }
    }

    UrlImage urlImage;
    urlImage.url = url;
    urlImage.path = path;

    insertUrlImageToList(urlImage);
}

void NotePreviewWidget::changeImageWidth()
{
    int widgetWidth = width();

    if (0 == widgetWidth) {
        return;
    }

    widgetWidth = (widgetWidth < 10) ? 0 : (width() - 10);
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
}

void NotePreviewWidget::downloadThreadStarted()
{
    for (auto &&item : mUrlImageList) {
        item.size = Tools::getImageSize(item.path);

        if (!item.size.isEmpty()) {
            continue;
        }

        if (gUrlResourceScheme.indexOf(QUrl(item.url).scheme()) != -1) {
            item.size = Tools::getImageSize(item.url);
            QByteArray data = Tools::downloadUrlToData(item.url);
            QPixmap pixmap;
            pixmap.loadFromData(data);
            Tools::writerFile(item.path, data);

            item.size = pixmap.size();
        }
    }

    changeImageWidth();
    mDownloadThread->exit();
}

void NotePreviewWidget::setDisplayHtml(const QString &html)
{
    setHtml(html);
    initUrlImageList();
}

void NotePreviewWidget::initUrlImageList()
{
    QRegularExpression re("<img src=\"([file|http|https|ftp]+:\\/\\/[^\"]+)\"( width=\"[0-9]*\")?");
    QRegularExpressionMatchIterator i = re.globalMatch(toHtml());
    while (i.hasNext()) {
        insertUrlImageToList(QUrl(i.next().captured(1)).toDisplayString());
    }

    mDownloadThread->start();
}
