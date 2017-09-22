#include "tagslistcell.h"
#include "ui_tagslistcell.h"
#include "globals.h"

TagsListCell::TagsListCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagsListCell)
{
    ui->setupUi(this);
}

TagsListCell::TagsListCell(TagsModel *tagsModel) : ui(new Ui::TagsListCell)
{
    ui->setupUi(this);

    mtagsModel = tagsModel;
    ui->lineEdit_nameTags->setText(mtagsModel->getName());
    ui->label_noteCount->setText(QString::number(mtagsModel->getCount()));
}

TagsListCell::~TagsListCell()
{
    delete ui;
}

TagsModel *TagsListCell::gettagsModel() const {
    return mtagsModel;
}

void TagsListCell::settagsModel(TagsModel *tagsModel) {
    mtagsModel = tagsModel;
}
