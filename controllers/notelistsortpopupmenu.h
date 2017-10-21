#ifndef NOTELISTSORTPOPUPMENU_H
#define NOTELISTSORTPOPUPMENU_H

#include <QMenu>
#include <QPushButton>
#include <QWidget>

class NoteListSortPopupMenu : public QMenu
{
    Q_OBJECT
public:
    explicit NoteListSortPopupMenu(QPushButton *pushButton, QWidget *parent = 0);
    void showEvent(QShowEvent* event);

private:
    QPushButton *pushButton;

};

#endif // NOTELISTSORTPOPUPMENU_H
