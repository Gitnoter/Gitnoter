#include "tagslistcell.h"
#include "ui_tagslistcell.h"
#include "globals.h"

TagsListCell::TagsListCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagsListCell)
{
    ui->setupUi(this);
}

TagsListCell::TagsListCell(TagsTableModel *tagsTableModel) : ui(new Ui::TagsListCell)
{
    ui->setupUi(this);

    mTagsTableModel = tagsTableModel;
    ui->lineEdit_nameTags->setText(mTagsTableModel->getName());
    ui->label_noteCount->setText(QString::number(mTagsTableModel->getCount()));
}

TagsListCell::~TagsListCell()
{
    delete ui;
}

TagsTableModel *TagsListCell::getTagsTableModel() const {
    return mTagsTableModel;
}

void TagsListCell::setTagsTableModel(TagsTableModel *tagsTableModel) {
    mTagsTableModel = tagsTableModel;
}
