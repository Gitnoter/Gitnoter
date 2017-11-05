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
    ui->pushButton_add->setHidden(true);
}

CategoryListWidget::~CategoryListWidget()
{
    delete ui;
}

void CategoryListWidget::init(const QString &category)
{
    mCategory = category;
    setCategoryList();
}

void CategoryListWidget::setCategoryList()
{
    ui->listWidget_data->clear();
    QList<CategoryModel *> categoryModelList = Globals::categoryModelList.getList();
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
    int showCount = 0;
    QString text = ui->lineEdit->displayText();
    QList<CategoryModel *> categoryModelList = Globals::categoryModelList.getList();

    for (int i = 0; i < categoryModelList.length(); ++i) {
        int index = categoryModelList[i]->getName().indexOf(text, 0, Qt::CaseInsensitive);
        QListWidgetItem *listWidgetItem = ui->listWidget_data->item(i);

        bool isHidden = text.isEmpty() ? false : index == -1;
        listWidgetItem->setHidden(isHidden);
        if (!isHidden) showCount += 1;
    }

    ui->pushButton_add->setHidden(showCount != 0);
}

void CategoryListWidget::on_listWidget_data_doubleClicked(const QModelIndex &index)
{
    on_buttonBox_accepted();
    emit accept();
}

void CategoryListWidget::on_pushButton_add_clicked()
{
    if (!ui->lineEdit->displayText().isEmpty()) {
        Globals::categoryModelList.append(ui->lineEdit->displayText());
        Globals::categoryModelList.saveToLocal();
        setCategoryList();
        ui->listWidget_data->sortItems(Qt::AscendingOrder);
        ui->lineEdit->clear();
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
