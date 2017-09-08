#include "tools.h"
#include "globals.h"

QFileInfoList Tools::getFilesPath(const QString path)
{
    QDir dir(path);
    QFileInfoList fileInfoList;

    foreach(QFileInfo mfi ,dir.entryInfoList()) {
        if(mfi.isFile()) {
            fileInfoList.append(mfi);
        }
        else {
            if(mfi.fileName() == "." || mfi.fileName() == "..") {
                continue;
            }
            fileInfoList += Tools::getFilesPath(mfi.absoluteFilePath());
        }
    }

    return fileInfoList;
}

QString Tools::getUuid() {
    return (QUuid::createUuid()).toString().mid(1, 36);
}

int Tools::timestampFromDateTime(const QString dateTime)
{
    return QDateTime::fromString(dateTime, g_dateFormat).toTime_t();
}

QString Tools::timestampToDateTime(int timestamp)
{
    return QDateTime::fromTime_t(static_cast<uint>(timestamp)).toString(g_dateFormat);
}

QString Tools::readerFile(QString path) {
    QFile textFile(path);
    textFile.open(QIODevice::ReadOnly);
    return QString(textFile.readAll());
}

QStringList Tools::textToStringList(QString text)
{
    QTextStream in(&text);
    QStringList stringList;

    while(!in.atEnd()) {
        stringList.append(in.readLine());
    }

    return stringList;
}

bool Tools::compareVersions(const QString string, const QString string1)
{
    QVersionNumber versionNumber = QVersionNumber::fromString(string);
    QVersionNumber versionNumber1 = QVersionNumber::fromString(string1);

    return versionNumber == versionNumber1;
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
        if (!split && string[i] == ":") {
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

QGraphicsDropShadowEffect* Tools::createShadowEffect()
{
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(40);
    effect->setColor(QColor(51, 51, 51));
    effect->setOffset(0, 0);

    return effect;
}


void Tools::changeWidgetBorder(QWidget *widget, const QString color, int width)
{
    widget->setStyleSheet(widget->styleSheet().replace(QRegExp("border-color: ?#[A-Z0-9]{6}"),"border-color: " + color));
    widget->setStyleSheet(widget->styleSheet().replace(QRegExp("border-width: ?[0-9]+px"), tr("border-width: %1px").arg(width)));
}

bool Tools::writerFile(QString path, QString text)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))  {
        return false;
    }

    QTextStream str(&f);
    str << text;

    f.close();

    return true;
}
