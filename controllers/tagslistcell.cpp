#include "tagslistcell.h"
#include "ui_tagslistcell.h"
#include "globals.h"

TagsListCell::TagsListCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagsListCell)
{
    ui->setupUi(this);
}

TagsListCell::TagsListCell(TagTableModel *tagTableModel) : ui(new Ui::TagsListCell)
{
    ui->setupUi(this);

    m_tagTableModel = tagTableModel;
    ui->lineEdit_name->setText(m_tagTableModel->getName());
    ui->label_noteCount->setText(QString::number(m_tagTableModel->getCount()));
}

TagsListCell::~TagsListCell()
{
    delete ui;
}

TagTableModel *TagsListCell::getTagTableModel() const {
    return m_tagTableModel;
}

void TagsListCell::setTagTableModel(TagTableModel *tagTableModel) {
    m_tagTableModel = tagTableModel;
}
