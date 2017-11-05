#include "ui_categorylistwidget.h"

#include "categorylistwidget.h"
#include "mainwindow.h"
#include "tools.h"
#include "globals.h"

CategoryListWidget::CategoryListWidget(QWidget *parent, QString category) :
        QDialog(parent),
        ui(new Ui::CategoryListWidget),
        mCategory(category)
{
    ui->setupUi(this);

    ui->listWidget_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->pushButton_addCategory->setHidden(true);
}

CategoryListWidget::~CategoryListWidget()
{
    delete ui;
}

void CategoryListWidget::init(const QString &category)
{
    mCategory = category;
    QList<CategoryModel *> categoryModelList = Globals::categoryModelList.getList();
    setCategoryList(categoryModelList);
    ui->lineEdit->clear();
}

void CategoryListWidget::setCategoryList(QList<CategoryModel *> categoryModelList)
{
    ui->listWidget_data->clear();
    for (int i = 0; i < categoryModelList.length(); ++i) {
        if (categoryModelList[i]->getName().isEmpty()) {
            continue;
        }
        QListWidgetItem *listWidgetItem = new QListWidgetItem(categoryModelList[i]->getName());
        listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(categoryModelList[i]));
        ui->listWidget_data->addItem(listWidgetItem);
        if (mCategory == categoryModelList[i]->getName()) {
            ui->listWidget_data->setItemSelected(listWidgetItem, true);
        }
    }
}

void CategoryListWidget::filtrateCategoryList()
{
    QList<CategoryModel *> categoryModelList = {};
    const QString text = ui->lineEdit->text();

    for (auto &&item : Globals::categoryModelList.getList()) {
        if (item->getName().isEmpty() || item->getName().indexOf(text, 0, Qt::CaseInsensitive) == -1) {
            continue;
        }
        categoryModelList.append(item);
    }

    setCategoryList(categoryModelList);
    ui->pushButton_addCategory->setHidden(categoryModelList.length() != 0);
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
        Globals::categoryModelList.saveToLocal();
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
        mCategory = (selectItemList[0]->data(Qt::UserRole).value<CategoryModel *>())->getName();
    }

    emit categoryChanged(mCategory);
}
