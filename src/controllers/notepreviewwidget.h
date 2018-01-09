#include "singletimeout.h"

#include <QTextBrowser>
#include <QResizeEvent>
#include <QThread>

struct UrlImage {
    QSize size;
    QString url;
    QString path;
};

class NotePreviewWidget : public QTextBrowser
{
    Q_OBJECT

public:
    explicit NotePreviewWidget(QWidget *parent = nullptr);

    void setDisplayHtml(const QString &html);

protected:
    void resizeEvent(QResizeEvent* event) override;
    QVariant loadResource(int type, const QUrl &name) override;

private:
    UrlImage getUrlImageByPath(const QString &path);
    UrlImage getUrlImageByUrl(const QString &url);
    void insertUrlImageToList(UrlImage urlImage);
    void insertUrlImageToList(const QString &url, QString path = "");
    void initUrlImageList();

private slots:
    void changeImageWidth();

    void downloadThreadStarted();

private:
    QList<UrlImage> mUrlImageList;
    SingleTimeout *mSingleTimeout;
    QTimer *mDownloadImageTimer;
    QThread *mDownloadThread;


};
