#ifndef NOTELISTSORTPOPUPMENU_H
#define NOTELISTSORTPOPUPMENU_H

#include "mainwindow.h"

#include <QMenu>
#include <QPushButton>
#include <QWidget>

class NoteListSortPopupMenu : public QMenu
{
    Q_OBJECT
public:
    explicit NoteListSortPopupMenu(QPushButton *pushButton, QWidget *parent = 0);
    void showEvent(QShowEvent* event);

signals:
    void actionTriggered();

private slots:
    void onActionTitleTriggered(bool triggered);
    void onActionCreateDateTriggered(bool triggered);
    void onActionUpdateDateTriggered(bool triggered);
    void onActionAscTriggered(bool triggered);
    void onActionDescTriggered(bool triggered);

private:
    void setBasisActionChecked(QAction *action);
    void setTypeActionChecked(QAction *action);

    void setPushButtonText(Gitnoter::SortBasis basis);
    void setPushButtonText(QAction *action);

public:
    QAction *action_title;
    QAction *action_createDate;
    QAction *action_updateDate;
    QAction *action_asc;
    QAction *action_desc;

private:
    MainWindow *mMainWindow;
    QPushButton *pushButton;

};

#endif // NOTELISTSORTPOPUPMENU_H
