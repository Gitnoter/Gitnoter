#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QDir>

class Tools : public QObject
{
    Q_OBJECT
public:
    static QFileInfoList getFilesPath(QString path);
    static QString getUuid();

    static QString readerFile(QString path);

    static int timestampFromDateTime(QString dateTime);
    static QString timestampToDateTime(int timestamp);

};

#endif // TOOLS_H