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

public slots:
    void onNoteListItemClicked(QListWidgetItem *item);

private:
    Ui::NoteListCellWidget *ui;
    NoteModel *noteModel;

    void setupUi();

};

#endif // NOTELISTCELLWIDGET_H
