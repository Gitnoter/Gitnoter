#include "tools.h"

#include <QDebug>
#include <QUuid>
#include <QDateTime>

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
    return QDateTime::fromString(dateTime, __DATE_FORMAT__).toTime_t();
}

QString Tools::timestampToDateTime(int timestamp)
{
    return QDateTime::fromTime_t(static_cast<uint>(timestamp)).toString(__DATE_FORMAT__);
}
