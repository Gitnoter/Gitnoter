#include "globals.h"
#include "tools.h"

#include <QStandardPaths>

const QString gPackageName = "com.huyaohui.gitnoter";
const QString gDateFormat = "yyyy/MM/dd hh:mm";
const QString gTagSplit = ", ";
const int gSevenDays = 7 * 24 * 60 * 60;

const QString gAppDataLocation = QDir(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath(gPackageName);
const QString gAppDataPath = QDir(gAppDataLocation).filePath("local/");
const QString gAppConfigPath = QDir(gAppDataPath).filePath("appconfig.xml");
const QString gAppLicensePath = QDir(gAppDataPath).filePath("license.gitnoter");
const QString gRepoPath = QDir(gAppDataLocation).filePath("user/user.git");
const QString gRepoName = "user.git";
const QString gRepoNameTemp = "user-temp.git";
const QString gRepoNoteTextPath = QDir(gRepoPath).filePath("notes/");
const QString gNoteTextFileName = "markdown.md";
const QString gNoteDataFileName = "data.json";
const QString gRepoNoteDataPath = QDir(gRepoPath).filePath("data");
const QString gRepoCategoryListPath = QDir(gRepoNoteDataPath).filePath("categories.list");
const QString gRepoTagListPath = QDir(gRepoNoteDataPath).filePath("tags.list");

const QString gCutRectKeySequence = "Ctrl+Shift+R";
const QString gCutFullKeySequence = "Ctrl+Shift+X";
const QString gCutWindowKeySequence = "Ctrl+Shift+D";

const int gEditorFontSize = 14;
const QString gFileScheme = "gnr";
const int gImageQuality = 85;

const QString gSearchEngine = QLocale::system().name() == "zh_CN" ? "https://www.baidu.com/s?wd="
                                                                  : "https://www.google.com/search?&q=";
const QString gGuideUrl = "http://gitnoter.github.io/";
const QString gHistoryChangeUrl = "http://gitnoter.github.io/";
const QString gMarkdownLanguageUrl = "http://gitnoter.github.io/";
const QString gPurchaseLicenseUrl = "http://gitnoter.github.io/";
const QString gIssueUrl = "https://github.com/Gitnoter/Gitnoter/issues";
const QString gEmailToUser = "gitnoter@gmail.com";

const QByteArray gAesKey = "Q5tO3jUB0oe7FmWq";
const QByteArray gAesIv  = "RNNSR8XNbMTuOSGd";

ConfigModel *gConfigModel = new ConfigModel();
GitManager *gGitManager = new GitManager();
ThemeManager *gThemeManager = new ThemeManager();

