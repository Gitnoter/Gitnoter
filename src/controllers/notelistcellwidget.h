#ifndef NOTELISTCELLWIDGET_H
#define NOTELISTCELLWIDGET_H

#include "notemodel.h"

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class NoteListCellWidget;
}

class NoteListCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NoteListCellWidget(NoteModel *noteModel, QWidget *parent = 0);
    ~NoteListCellWidget();

    void reloadData();

    virtual void paintEvent(QPaintEvent *event);

private:
    Ui::NoteListCellWidget *ui;
    NoteModel *mNoteModel;

};

#endif // NOTELISTCELLWIDGET_H
