#ifndef GLOBALS_H
#define GLOBALS_H

#include "notemodel.h"
#include "configmodel.h"
#include "gitmanager.h"
#include "thememanager.h"

#include <QObject>

extern const QString gPackageName;
extern const QString gDateFormat;
extern const QString gTagSplit;
extern const int gSevenDays;

extern const QString gAppDataLocation;
extern const QString gAppDataPath;
extern const QString appConfigPath;
extern const QString gRepoPath;
extern const QString gRepoNoteTextPath;
extern const QString gNoteTextFileName;
extern const QString gNoteDataFileName;
extern const QString gRepoNoteDataPath;
extern const QString gRepoCategoryListPath;
extern const QString gRepoTagListPath;

extern const QString gLockWindowKeySequence;
extern const QString gCutRectKeySequence;
extern const QString gCutFullKeySequence;
extern const QString gCutWindowKeySequence;

extern const int gEditorFontSize;
extern const QString gFileScheme;
extern const int gImageQuality;

extern const QString gSearchEngine;
extern const QString gGuideUrl;
extern const QString gHistoryChangeUrl;
extern const QString gMarkdownLanguageUrl;
extern const QString gIssueUrl;
extern const QString gEmailToUser;

extern const QByteArray gAesKey;
extern const QByteArray gAesIv;

extern ConfigModel *gConfigModel;
extern GitManager *gGitManager;
extern ThemeManager *gThemeManager;

#endif // GLOBALS_H
