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
    ui->label_datetime->setText(Tools::timestampToDateTime(noteModel->getCreateDate()));
    ui->label_datetime->setText(Tools::timestampToDateTime(noteModel->getUpdateDate()));
}

NoteListCellWidget::~NoteListCellWidget()
{
    delete ui;
}

void NoteListCellWidget::onNoteListItemClicked(QListWidgetItem *item)
{
    NoteModel *noteModel = item->data(Qt::UserRole).value<NoteModel *>();
    Global::themeManager->setTheme(this, noteModel == this->noteModel
                                   ? ThemeManager::NoteListCellWidgetSelected
                                   : ThemeManager::NoteListCellWidgetUnselected);
}

void NoteListCellWidget::setupUi()
{
    Global::themeManager->setTheme(this, ThemeManager::NoteListCellWidgetUnselected);
    connect(parent(), SIGNAL(noteListItemClicked(QListWidgetItem *)), this, SLOT(onNoteListItemClicked(QListWidgetItem *)));
}
