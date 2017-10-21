#ifndef NOTELISTCELLWIDGET_H
#define NOTELISTCELLWIDGET_H

#include "notemodel.h"

#include <QWidget>

namespace Ui {
class NoteListCellWidget;
}

class NoteListCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NoteListCellWidget(NoteModel *noteModel, QWidget *parent = 0);
    ~NoteListCellWidget();

private:
    Ui::NoteListCellWidget *ui;
    NoteModel *noteModel;

};

#endif // NOTELISTCELLWIDGET_H
