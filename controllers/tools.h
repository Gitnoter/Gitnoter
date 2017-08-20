#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QDir>

#define __DATE_FORMAT__ "yyyy/MM/dd hh:mm:ss"

class Tools : public QObject
{
    Q_OBJECT
public:
    static QFileInfoList getFilesPath(QString path);
    static QString getUuid();

    static int timestampFromDateTime(QString dateTime);
    static QString timestampToDateTime(int timestamp);

};

#endif // TOOLS_H