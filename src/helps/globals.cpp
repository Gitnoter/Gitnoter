#include "globals.h"

#include <QStandardPaths>

const QString gAppStandardPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
const QString gPackageName = "com.huyaohui.gitnoter";

ConfigModel *gConfigModel = new ConfigModel();
ThemeManager *gThemeManager = new ThemeManager();

