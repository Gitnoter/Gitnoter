#include "ui_tagcellwidget.h"

#include "tagcellwidget.h"

TagCellWidget::TagCellWidget(const QString tagName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagCellWidget),
    tagName(tagName)
{
    ui->setupUi(this);
    ui->pushButton_tag->setText(tagName);

    setFixedHeight(14);
    connect(this, SIGNAL(clicked(QString)), parent, SLOT(onTagCellWidgetClicked(QString)));
    connect(this, SIGNAL(clicked(QString)), parent, SLOT(onTagCellWidgetClicked(QString)));
}

TagCellWidget::~TagCellWidget()
{
    delete ui;
}

QString TagCellWidget::getTagName() const
{
    return tagName;
}

void TagCellWidget::on_pushButton_tag_clicked()
{
    emit clicked(tagName);
}

void TagCellWidget::on_pushButton_delete_clicked()
{
    emit clicked(tagName);
}
