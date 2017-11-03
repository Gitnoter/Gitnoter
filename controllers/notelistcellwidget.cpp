#include "notelistcellwidget.h"
#include "ui_notelistcellwidget.h"

#include "tools.h"

NoteListCellWidget::NoteListCellWidget(NoteModel *noteModel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteListCellWidget),
    mNoteModel(noteModel)
{
    ui->setupUi(this);

    ui->label_title->setText(noteModel->getTitle());
    ui->frame_body->setText(noteModel->getNoteText());
    ui->label_categoryList->setText(noteModel->getCategory());
    ui->label_tagList->setText(noteModel->getStringFormTagList());
}

NoteListCellWidget::~NoteListCellWidget()
{
    delete ui;
}
