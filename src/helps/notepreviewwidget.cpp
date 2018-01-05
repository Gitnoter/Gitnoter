#include "notepreviewwidget.h"
#include "tools.h"
#include "globals.h"

#include <QDebug>

NotePreviewWidget::NotePreviewWidget(QWidget *parent) : QTextBrowser(parent) {
    QDir().mkpath(gTempPath);
}

void NotePreviewWidget::resizeEvent(QResizeEvent* event) {
    emit resize(event->size(), event->oldSize());

    // we need this, otherwise the preview is always blank
    QTextBrowser::resizeEvent(event);
}

QVariant NotePreviewWidget::loadResource(int type, const QUrl &name)
{
    if (2 == type) {
        const QString filePath = QDir(gTempPath).filePath("note-browser-" + Tools::md5(name.toString()) + ".jpg");
        UrlImage urlImage = getUrlImageByUrl(name.toString());

        if (urlImage.url.isEmpty()) {
            QPixmap pixmap;
            QByteArray data = Tools::readerFile(filePath).toUtf8();

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
        if (item.path == url) {
            return item;
        }
    }

    return UrlImage();
}
