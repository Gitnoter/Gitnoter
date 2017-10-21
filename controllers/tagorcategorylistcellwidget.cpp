#include "tagorcategorylistcellwidget.h"
#include "ui_tagorcategorylistcellwidget.h"

TagOrCategoryListCellWidget::TagOrCategoryListCellWidget(QString text, QString color, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagOrCategoryListCellWidget)
{
    ui->setupUi(this);

    ui->label->setStyleSheet(tr("color: %1;").arg(color));
    ui->label->setText(text);
}

TagOrCategoryListCellWidget::~TagOrCategoryListCellWidget()
{
    delete ui;
}
