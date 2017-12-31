#include "thememanager.h"

#include <QFile>

ThemeManager::ThemeManager()
{

}

void ThemeManager::setTheme(QWidget *widget, ThemeFlag theme)
{
    QString stylePath;
    if (theme == Default) {
        stylePath = ":/theme/default.qss";
    }

    QFile file(stylePath);

    if (file.open(QIODevice::ReadOnly)) {
        widget->setStyleSheet(QLatin1String(file.readAll()));
    }

    file.close();
}
