#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QDir>

class Tools : public QObject
{
public:
    static QFileInfoList getFilesPath(QString path);
    static QString getUuid();

    static QString readerFile(QString path);
    static QStringList textToStringList(QString text);

    static int timestampFromDateTime(QString dateTime);
    static QString timestampToDateTime(int timestamp);

    static bool compareVersions(const QString string, const QString string1);
};

#endif // TOOLS_H