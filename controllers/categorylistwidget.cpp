#include "ui_categorylistwidget.h"

#include "categorylistwidget.h"
#include "mainwindow.h"
#include "tools.h"
#include "globals.h"

CategoryListWidget::CategoryListWidget(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CategoryListWidget)
{
    ui->setupUi(this);

    ui->listWidget_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->pushButton_addCategory->setHidden(true);
}

CategoryListWidget::~CategoryListWidget()
{
    delete ui;
}

void CategoryListWidget::init(GroupTreeWidget *groupTreeWidget, const QString &category)
{
    setCategoryList(groupTreeWidget->getGroupModelList(Gitnoter::Category));
    ui->lineEdit->clear();
}

void CategoryListWidget::setCategoryList(QList<GroupModel *> groupModelList)
{
    ui->listWidget_data->clear();
    for (int i = 0; i < groupModelList.length(); ++i) {
        if (groupModelList[i]->getName().isEmpty()) {
            continue;
        }
        QListWidgetItem *listWidgetItem = new QListWidgetItem(groupModelList[i]->getName());
        listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(groupModelList[i]));
        ui->listWidget_data->addItem(listWidgetItem);
        if (mCategory == groupModelList[i]->getName()) {
            ui->listWidget_data->setItemSelected(listWidgetItem, true);
        }
    }
}

void CategoryListWidget::filtrateCategoryList()
{
    const QString text = ui->lineEdit->text();
    bool addButtonHidden = true;
    for (int i = 0; i < ui->listWidget_data->count(); ++i) {
        QListWidgetItem *listWidgetItem = ui->listWidget_data->item(i);
        listWidgetItem->setHidden(
                listWidgetItem->text().isEmpty() || listWidgetItem->text().indexOf(text, 0, Qt::CaseInsensitive) == -1);
        addButtonHidden = false;
    }

    ui->pushButton_addCategory->setHidden(addButtonHidden);
}

void CategoryListWidget::on_listWidget_data_doubleClicked(const QModelIndex &)
{
    on_buttonBox_accepted();
    emit accept();
}

void CategoryListWidget::on_pushButton_addCategory_clicked()
{
    const QString text = ui->lineEdit->text();
    if (!text.isEmpty()) {
        emit categoryAppend(text);
        filtrateCategoryList();
        ui->listWidget_data->sortItems(Qt::AscendingOrder);
    }
}

void CategoryListWidget::on_lineEdit_textChanged(const QString &)
{
    filtrateCategoryList();
}

void CategoryListWidget::on_buttonBox_accepted()
{
    auto selectItemList = ui->listWidget_data->selectedItems();
    if (selectItemList.length() > 0) {
        mCategory = (selectItemList[0]->data(Qt::UserRole).value<GroupModel *>())->getName();
        emit categoryChanged(mCategory);
    }
}
