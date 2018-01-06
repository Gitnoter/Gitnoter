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

protected:
    void resizeEvent(QResizeEvent* event) override;
    QVariant loadResource(int type, const QUrl &name) override;

private:
    UrlImage getUrlImageByPath(const QString &path);
    UrlImage getUrlImageByUrl(const QString &url);

signals:
    void resize(QSize size, QSize oldSize = QSize());

public slots:
    void setHtml(const QString &text);

private slots:
    void changeImageWidth();

    void downloadWebImageTimer();

    void downloadThreadStarted();

private:
    QList<UrlImage> mUrlImageList;
    bool mResizeWindow;
    QTimer *mDownloadImageTimer;
    QThread *mDownloadThread;

};
