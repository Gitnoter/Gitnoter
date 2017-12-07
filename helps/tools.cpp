#include "tools.h"
#include "globals.h"

#include <QTcpSocket>
#include <QUuid>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QTimer>
#include <QNetworkReply>

QFileInfoList Tools::getFilesPath(const QString path)
{
    QDir dir(path);
    QFileInfoList fileInfoList;

    for (auto &&mfi : dir.entryInfoList()) {
        if (mfi.isFile()) {
            fileInfoList.append(mfi);
        }
        else {
            if (mfi.fileName() == "." || mfi.fileName() == "..") {
                continue;
            }
            fileInfoList += Tools::getFilesPath(mfi.absoluteFilePath());
        }
    }

    return fileInfoList;
}

QString Tools::getUuid()
{
    return (QUuid::createUuid()).toString().mid(1, 36);
}

QString Tools::getShortUuid()
{
    return (QUuid::createUuid()).toString().mid(1, 8);
}

int Tools::timestampFromDateTime(const QString dateTime)
{
    return QDateTime::fromString(dateTime, gDateFormat).toTime_t();
}

QString Tools::timestampToDateTime(int timestamp)
{
    return QDateTime::fromTime_t(static_cast<uint>(timestamp)).toString(gDateFormat);
}

QString Tools::readerFile(QString path)
{
    QFile textFile(path);
    textFile.open(QIODevice::ReadOnly);
    return QString(textFile.readAll());
}

QStringList Tools::readerFileToList(QString path)
{
    QString str = readerFile(path);
    QTextStream in(&str);
    QStringList stringList;

    while (!in.atEnd()) {
        stringList.append(in.readLine().trimmed());
    }

    return stringList;
}

QStringList Tools::textToStringList(QString text)
{
    QTextStream in(&text);
    QStringList stringList;

    while (!in.atEnd()) {
        stringList.append(in.readLine());
    }

    return stringList;
}

QStringList Tools::splitNotesData(QString string)
{
    bool split = false;
    QStringList list;

    if (string.isEmpty()) {
        return list;
    }

    list << "" << "";

    for (int i = 0; i < string.length(); ++i) {
        if (!split && QString::QString(string[i]) == ":") {
            split = true;
            continue;
        }

        if (!split) {
            list[0] += string[i];
        }
        else {
            list[1] += string[i];
        }
    }
    return list;
}

QGraphicsDropShadowEffect *Tools::createShadowEffect()
{
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(40);
    effect->setColor(QColor(51, 51, 51));
    effect->setOffset(0, 0);

    return effect;
}


void Tools::changeWidgetBorder(QWidget *widget, const QString color, int width)
{
    widget->setStyleSheet(
            widget->styleSheet().replace(QRegExp("border-color: ?#[A-Z0-9]{6}"), "border-color: " + color));
    widget->setStyleSheet(
            widget->styleSheet().replace(QRegExp("border-width: ?[0-9]+px"), tr("border-width: %1px").arg(width)));
}

bool Tools::writerFile(QString path, const QByteArray &data)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        return false;
    }

    f.write(data);
    f.close();

    return true;
}

int Tools::rowShowCount(QTableWidget *tableWidget)
{
    int rowShowCount = 0;
    int listRowCount = tableWidget->rowCount();
    for (int i = 0; i < listRowCount; ++i) {
        if (tableWidget->isRowHidden(i)) {
            rowShowCount += 1;
        }
    }
    return listRowCount - rowShowCount;
}

int Tools::rowShowCount(QListWidget *listWidget)
{
    int rowShowCount = 0;
    int listRowCount = listWidget->count();
    for (int i = 0; i < listRowCount; ++i) {
        if (listWidget->isRowHidden(i)) {
            rowShowCount += 1;
        }
    }
    return listRowCount - rowShowCount;
}

bool Tools::createMkDir(const QString &path)
{
    QDir dir(path);
    if(!dir.exists()) {
        return QDir().mkpath(path); //创建多级目录
    }

    return false;
}

bool Tools::urlExists(QString urlString) {
    QUrl theurl(urlString);
//    QTextStream out(stdout);
    QTcpSocket socket;
    QByteArray buffer;

    socket.connectToHost(theurl.host(), 80);
    if (socket.waitForConnected()) {
        //Standard http request
        socket.write("GET / HTTP/1.1\r\n"
                             "host: " + theurl.host().toUtf8() + "\r\n\r\n");
        if (socket.waitForReadyRead()) {
            while(socket.bytesAvailable()){
                buffer.append(socket.readAll());
                int packetSize=buffer.size();
                while(packetSize>0)
                {
                    //Output server response for debugging
//                    out << "[" << buffer.data() << "]" <<endl;

                    //set Url if 200, 301, or 302 response given assuming that server will redirect
                    if (buffer.contains("200 OK") ||
                        buffer.contains("302 Found") ||
                        buffer.contains("301 Moved")) {
                        return true;
                    }
                    buffer.remove(0,packetSize);
                    //packetSize=getPacketSize(buffer);
                    packetSize=buffer.size();

                } //while packet size >0
            } //while socket.bytesavail

        } //socket wait for ready read
    }//socket write
    return false;
}

/**
 * Downloads an url and returns the data
 *
 * @param url
 * @return {QByteArray} the content of the downloaded url
 */
QByteArray Tools::downloadUrlToData(QUrl url) {
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QNetworkRequest networkRequest = QNetworkRequest(url);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute,
                                true);
#endif

    QByteArray data;
    QNetworkReply *reply = manager->get(networkRequest);
    loop.exec();

    // if we didn't get a timeout let us return the content
    if (timer.isActive()) {
        int statusCode = reply->attribute(
                QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // only get the data if the status code was "success"
        // see: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
        if (statusCode >= 200 && statusCode < 300) {
            // get the data from the network reply
            data = reply->readAll();
        }
    }

    return data;
}

QString Tools::firstUpperCase(QString text)
{
    QRegularExpression wordSplitter("(?<=[\\s\\v])");

    QStringList words = text.toLower().split(wordSplitter);

    for (QString & word : words) {
        if (word.length() > 0) {
            word = word.left(1).toUpper() + word.right(word.length() - 1);
        }
    }

    return words.join("");
}
