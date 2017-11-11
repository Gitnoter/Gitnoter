#ifndef GLOBALS_H
#define GLOBALS_H

#include "notemodel.h"
#include "configmodel.h"
#include "gitmanager.h"
#include "thememanager.h"

#include <QObject>

class Globals : public QObject
{
Q_OBJECT

public:
    static const QString packageName;
    static const QString version;
    static const QString dateFormat;
    static const QString tagSplit;
    static const int sevenDays;

    static const QString appDataLocation;
    static const QString appDataPath;
    static const QString appConfigPath;
    static const QString repoPath;
    static const QString repoNoteTextPath;
    static const QString noteTextFileName;
    static const QString noteDataFileName;
    static const QString repoNoteDataPath;
    static const QString repoCategoryListPath;
    static const QString repoTagListPath;

    static const QString newNoteKeySequence;
    static const QString lockWindowKeySequence;
    static const QString cutWindowKeySequence;

    static const QByteArray aesKey;
    static const QByteArray aesIv;

    static ConfigModel *configModel;
    static GitManager *gitManager;
    static ThemeManager *themeManager;

};

#endif // GLOBALS_H
