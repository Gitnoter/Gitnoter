#include "tools.h"
#include "globals.h"

#include <QTcpSocket>
#include <QUuid>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QTimer>
#include <QNetworkReply>
#include <QCryptographicHash>

#include <qtinyaes/qtinyaes.h>

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

int Tools::timestampFromDateTime(const QString dateTime, const QString &format)
{
    return QDateTime::fromString(dateTime, format.isEmpty() ? __DateFormat__ : format).toTime_t();
}

QString Tools::timestampToDateTime(int timestamp)
{
    return QDateTime::fromTime_t(static_cast<uint>(timestamp)).toString(__DateFormat__);
}

QString Tools::readerFileString(QString path)
{
    return QString(readerFile(path));
}

QByteArray Tools::readerFile(QString path)
{
    path = fileUrlToPath(path);

    QFile textFile(path);
    textFile.open(QIODevice::ReadOnly);

    if (!textFile.isReadable()) {
        return "";
    }

    return textFile.readAll();
}

QStringList Tools::readerFileToList(QString path)
{
    QString str = readerFileString(path);
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
        if (!split && QString(string[i]) == ":") {
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


void Tools::changeWidgetBorder(QWidget *widget, const QString &color, int width)
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

QString Tools::getUsernameByEmail(const QString &email)
{
    QStringList stringList = email.split("@");

    return stringList.length() > 0 ? stringList[0] : "";
}

QString Tools::encrypt(const QString &string)
{
    QTinyAes aes(QTinyAes::CBC, __AES_KEY__, __AES_IV__);
    return aes.encrypt(string.toUtf8()).toBase64();
}

QString Tools::decrypt(const QString &string)
{
    QTinyAes aes(QTinyAes::CBC, __AES_KEY__, __AES_IV__);

    return QString(aes.decrypt(QByteArray::fromBase64(string.toUtf8())));
}

const char *Tools::qstringToConstData(const QString &string)
{
    return (new QByteArray(string.toLatin1()))->data();
}

QString Tools::md5(const QString &text)
{
    return QString(QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Md5).toHex());
}

QSize Tools::getImageSize(const QString &path)
{
    const QString filePath = fileUrlToPath(path);
#ifdef Q_OS_WIN
    QPixmap pixmap;
    QByteArray data = readerFile(filePath);

    pixmap.loadFromData(data);

    return pixmap.size();
#endif
    int the_x = 0;
    int the_y = 0;

    getImageSize(qstringToConstData(filePath), &the_x, &the_y);

    return {the_x, the_y};
}


bool Tools::getImageSize(const char *fn, int *x,int *y)
{
    FILE *f=fopen(fn,"rb");
    if (f==0) return false;
    fseek(f,0,SEEK_END);
    long len=ftell(f);
    fseek(f,0,SEEK_SET);
    if (len<24) {
        fclose(f);
        return false;
    }
    // cout << fn << endl;
    // Strategy:
    // reading GIF dimensions requires the first 10 bytes of the file
    // reading PNG dimensions requires the first 24 bytes of the file
    // reading JPEG dimensions requires scanning through jpeg chunks
    // In all formats, the file is at least 24 bytes big, so we'll read that always
    unsigned char buf[24]; fread(buf,1,24,f);

    // For JPEGs, we need to read the first 12 bytes of each chunk.
    // We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
    if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF && buf[3]==0xE0 && buf[6]=='J' && buf[7]=='F' && buf[8]=='I' && buf[9]=='F')
    { long pos=2;
        while (buf[2]==0xFF)
        { if (buf[3]==0xC0 || buf[3]==0xC1 || buf[3]==0xC2 || buf[3]==0xC3 || buf[3]==0xC9 || buf[3]==0xCA || buf[3]==0xCB) break;
            pos += 2+(buf[4]<<8)+buf[5];
            if (pos+12>len) break;
            fseek(f,pos,SEEK_SET); fread(buf+2,1,12,f);
        }
    }

    fclose(f);

    // JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
    if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF)
    { *y = (buf[7]<<8) + buf[8];
        *x = (buf[9]<<8) + buf[10];
        //cout << *x << endl;
        return true;
    }

    // GIF: first three bytes say "GIF", next three give version number. Then dimensions
    if (buf[0]=='G' && buf[1]=='I' && buf[2]=='F')
    { *x = buf[6] + (buf[7]<<8);
        *y = buf[8] + (buf[9]<<8);
        return true;
    }

    // PNG: the first frame is by definition an IHDR frame, which gives dimensions
    if ( buf[0]==0x89 && buf[1]=='P' && buf[2]=='N' && buf[3]=='G' && buf[4]==0x0D && buf[5]==0x0A && buf[6]==0x1A && buf[7]==0x0A
         && buf[12]=='I' && buf[13]=='H' && buf[14]=='D' && buf[15]=='R')
    { *x = (buf[16]<<24) + (buf[17]<<16) + (buf[18]<<8) + (buf[19]<<0);
        *y = (buf[20]<<24) + (buf[21]<<16) + (buf[22]<<8) + (buf[23]<<0);
        return true;
    }

    return false;
}

QString Tools::fileUrlToPath(const QString &url)
{
    if (url.startsWith("file://")) {
        int position = 7;
#ifdef Q_OS_WIN
        position = 8;
#endif
        return url.mid(position);
    }

    return url;
}

QString Tools::fileToBase64(const QString &path)
{
    return QString(readerFile(path).toBase64());
}
