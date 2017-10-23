#include "thememanager.h"
#include "globals.h"

#include "QFile"

ThemeManager::ThemeManager()
{

}

void ThemeManager::setTheme(QWidget *widget, ThemeFlag theme)
{
    QString stylePath;
    if (theme == Default) {
        stylePath = ":/theme/default.qss";
    }

    if (theme == NoteListCellWidgetSelected) {
        if (Global::configModel->getTheme() == Default) {
            stylePath = ":/theme/notelistcellwidget-selected.qss";
        }
    }
    else if (theme == NoteListCellWidgetUnselected) {
        if (Global::configModel->getTheme() == Default) {
            stylePath = ":/theme/notelistcellwidget-unselected.qss";
        }
    }

    QFile file(stylePath);

    if (file.open(QIODevice::ReadOnly)) {
        widget->setStyleSheet(QLatin1String(file.readAll()));
    }

    file.close();
}
