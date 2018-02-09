#ifndef GLOBALS_H
#define GLOBALS_H

#include "notemodel.h"
#include "configmodel.h"
#include "gitmanager.h"
#include "thememanager.h"

#include <QObject>

extern const QString gAppStandardPath;
extern const QString gPackageName;

extern ConfigModel *gConfigModel;
extern ThemeManager *gThemeManager;

#define __DateFormat__  "yyyy/MM/dd hh:mm"
#define __TagSplit__  ", "
#define __RecentlyUseDay__ (7 * 24 * 60 * 60)
#define __AES_KEY__ "Q5tO3jUB0oe7FmWq"
#define __AES_IV__ "RNNSR8XNbMTuOSGd"
#define __GOOGLE_ANALYTICS_TRACKING_ID__ "UA-112372815-1"

#define __AppDataLocation__         (gAppStandardPath + "/" + gPackageName)
#define __AppDataPath__             (gAppStandardPath + "/" + gPackageName + "/local")
#define __AppConfigPath__           (gAppStandardPath + "/" + gPackageName + "/local" + "/appconfig.xml")
#define __AppLicensePath__          (gAppStandardPath + "/" + gPackageName + "/local" + "/license.gitnoter")
#define __RepoPath__                (gAppStandardPath + "/" + gPackageName + "/user/user.git")
#define __RepoNoteTextPath__        (gAppStandardPath + "/" + gPackageName + "/user/user.git" + "/notes")
#define __RepoNoteDataPath__        (gAppStandardPath + "/" + gPackageName + "/user/user.git" + "/data")
#define __RepoCategoryListPath__    (gAppStandardPath + "/" + gPackageName + "/user/user.git" + "/data" + "/categories.list")
#define __RepoTagListPath__         (gAppStandardPath + "/" + gPackageName + "/user/user.git" + "/data" + "/tags.list")
#define __TempPath__                (gAppStandardPath + "/" + gPackageName + "/temp")
#define __CRASHES_PATH__            (gAppStandardPath + "/" + gPackageName + "/crashes")
#define __CRASHES_BAK_PATH__        (gAppStandardPath + "/" + gPackageName + "/crashes_bak")
#define __RepoName__                "user.git"
#define __RepoNameTemp__            "user-temp.git"
#define __NoteTextFileName__        "markdown.md"
#define __NoteDataFileName__        "data.json"

#define __CutRectKeySequence__   QString("Ctrl+Shift+R")
#define __CutFullKeySequence__   QString("Ctrl+Shift+X")
#define __CutWindowKeySequence__ QString("Ctrl+Shift+D")

#define __EditorFontSize__      14
#define __ImageQuality__        85
#define __FileScheme__          QString("gnr")
#define __UrlResourceScheme__   QStringList({ "http", "https", "ftp" })

#define __SearchEngine__        (QLocale::system().name() == "zh_CN" ? "https://www.baidu.com/s?wd=" : "https://www.google.com/search?&q=")
#define __GuideUrl__            "http://www.gitnoter.com/zh/update/"
#define __HistoryChangeUrl__    "http://www.gitnoter.com/zh/update/"
#define __MarkdownLanguageUrl__ "https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet"
#define __PurchaseLicenseUrl__  "http://www.gitnoter.com/zh/buy/"
#define __IssueUrl__            "https://github.com/Gitnoter/Gitnoter/issues"
#define __EmailToUser__         "gitnoter@gmail.com"

#endif // GLOBALS_H
