#include "categorylistwidget.h"
#include "mainwindow.h"
#include "ui_categorylistwidget.h"
#include "tools.h"
#include "globals.h"

#include <QDebug>

CategoryListWidget::CategoryListWidget(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CategoryListWidget)
{
    ui->setupUi(this);

    ui->listWidget_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->pushButton_add->setHidden(true);

    mSetListWidgetList();
}

CategoryListWidget::~CategoryListWidget()
{
    delete ui;
}

void CategoryListWidget::mSetListWidgetList()
{
    ui->listWidget_data->clear();
    for (int i = 0; i < Global::categoryModelList.length(); ++i) {
        ui->listWidget_data->addItem(Global::categoryModelList[i]->getName());
        ui->listWidget_data->item(i)->setData(Qt::UserRole, QVariant::fromValue(Global::categoryModelList[i]));
        if (Global::openNoteModel->getCategory() == Global::categoryModelList[i]->getName()) {
            ui->listWidget_data->setItemSelected(ui->listWidget_data->item(i), true);
        }
    }
}

void CategoryListWidget::mFiltrateListWidgetList()
{
    int showCount = 0;
    QString text = ui->lineEdit->displayText();
    for (int i = 0; i < Global::categoryModelList.length(); ++i) {
        int index = Global::categoryModelList[i]->getName().indexOf(text, 0, Qt::CaseInsensitive);
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
        Global::categoryModelList.append(new CategoryModel(ui->lineEdit->displayText()));
        Global::saveCategoryModelList();
        mSetListWidgetList();
        ui->listWidget_data->sortItems(Qt::AscendingOrder);
        ui->lineEdit->clear();
    }
}

void CategoryListWidget::on_lineEdit_textChanged(const QString &arg1)
{
    mFiltrateListWidgetList();
}

void CategoryListWidget::on_buttonBox_accepted()
{
    auto selectItemList = ui->listWidget_data->selectedItems();
    for (auto &&item : selectItemList) {
        Global::openNoteModel->setCategory((item->data(Qt::UserRole).value<CategoryModel *>())->getName());
    }

    emit categoriesChanged();
}
