#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QDir>
#include <QGraphicsDropShadowEffect>
#include <QWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QUrl>

class Tools : public QObject
{
public:
    static QFileInfoList getFilesPath(QString path);

    static QString getUuid();

    static QString getShortUuid();

    static QString md5(const QString &text);

    static QString readerFileString(QString path);

    static QByteArray readerFile(QString path);

    static QStringList readerFileToList(QString path);

    static bool writerFile(QString path, const QByteArray &data);

    static QStringList textToStringList(QString text);

    static bool createMkDir(const QString &path);

    static int timestampFromDateTime(QString dateTime, const QString &format = "");

    static QString timestampToDateTime(int timestamp);

    static QStringList splitNotesData(QString string);

    static QGraphicsDropShadowEffect *createShadowEffect();

    static void changeWidgetBorder(QWidget *widget, const QString &color, int width);

    static int rowShowCount(QTableWidget *tableWidget);

    static int rowShowCount(QListWidget *listWidget);

    static bool urlExists(QString urlString);

    static QByteArray downloadUrlToData(QUrl url);

    static QString firstUpperCase(QString text);

    static QString getUsernameByEmail(const QString &email);

    static QString encrypt(const QString &string);

    static QString decrypt(const QString &string);

    static const char *qstringToConstData(const QString &string);

    static QSize getImageSize(const QString &path);

    static QString fileUrlToPath(const QString &url);

private:
    /**
     * http://www.cplusplus.com/forum/beginner/45217/
     *
     * @param fn
     * @param x
     * @param y
     * @return
     */
    static bool getImageSize(const char *fn, int *x,int *y);

};

#endif // TOOLS_H
