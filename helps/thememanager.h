#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QWidget>

class ThemeManager : public QObject
{
    Q_OBJECT
public:
    ThemeManager();

    enum ThemeFlag {
        Default = 0x0000,

        // action switch theme
        NoteListCellWidgetUnselected = 0x0100,
        NoteListCellWidgetSelected = 0x0101,
    };

    void setTheme(QWidget *widget, ThemeFlag theme);

};

#endif // THEMEMANAGER_H
