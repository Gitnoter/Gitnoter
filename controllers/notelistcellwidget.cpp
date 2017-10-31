#include "notelistcellwidget.h"
#include "ui_notelistcellwidget.h"

#include "globals.h"
#include "tools.h"

NoteListCellWidget::NoteListCellWidget(NoteModel *noteModel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteListCellWidget),
    noteModel(noteModel)
{
    ui->setupUi(this);
    setupUi();

    ui->label_title->setText(noteModel->getTitle());
    ui->frame_body->setText(noteModel->getNoteText());
    ui->label_categoryList->setText(noteModel->getCategory());
    ui->label_tagList->setText(noteModel->getStringFormTagList());
}

NoteListCellWidget::~NoteListCellWidget()
{
    delete ui;
}

void NoteListCellWidget::onNoteListItemClicked(QListWidgetItem *item)
{
    NoteModel *noteModel = item->data(Qt::UserRole).value<NoteModel *>();
    setStyleSheet(noteModel == this->noteModel ? "QWidget#widget_content{border-bottom:none;}" : "");
}

void NoteListCellWidget::setupUi()
{
    connect(parent(), SIGNAL(noteListItemClicked(QListWidgetItem *)), this, SLOT(onNoteListItemClicked(QListWidgetItem *)));
}
