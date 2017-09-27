#include "categorieswidget.h"
#include "mainwindow.h"
#include "ui_categorieswidget.h"
#include "tools.h"
#include "globals.h"

#include <QDebug>

CategoriesWidget::CategoriesWidget(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CategoriesWidget)
{
    ui->setupUi(this);

    ui->listWidget_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->pushButton_add->setHidden(true);

    mSetListWidgetList();
}

CategoriesWidget::~CategoriesWidget()
{
    delete ui;
}

void CategoriesWidget::mSetListWidgetList()
{
    ui->listWidget_data->clear();
    for (int i = 0; i < Global::categoriesModelList.length(); ++i) {
        ui->listWidget_data->addItem(Global::categoriesModelList[i]->getName());
        ui->listWidget_data->item(i)->setData(Qt::UserRole, QVariant::fromValue(Global::categoriesModelList[i]));
        if (Global::openNoteModel->categoriesModel->getName() == Global::categoriesModelList[i]->getName()) {
            ui->listWidget_data->setItemSelected(ui->listWidget_data->item(i), true);
        }
    }
}

void CategoriesWidget::mFiltrateListWidgetList()
{
    int showCount = 0;
    QString text = ui->lineEdit->displayText();
    for (int i = 0; i < Global::categoriesModelList.length(); ++i) {
        int index = Global::categoriesModelList[i]->getName().indexOf(text, 0, Qt::CaseInsensitive);
        QListWidgetItem *listWidgetItem = ui->listWidget_data->item(i);

        bool isHidden = text.isEmpty() ? false : index == -1;
        listWidgetItem->setHidden(isHidden);
        if (!isHidden) showCount += 1;
    }

    ui->pushButton_add->setHidden(showCount != 0);
}

void CategoriesWidget::on_listWidget_data_doubleClicked(const QModelIndex &index)
{
    on_buttonBox_accepted();
    emit accept();
}

void CategoriesWidget::on_pushButton_add_clicked()
{
    if (!ui->lineEdit->displayText().isEmpty()) {
        Global::categoriesModelList.append(new CategoriesModel(ui->lineEdit->displayText()));
        Global::addCategoriesModelList();
        mSetListWidgetList();
        ui->listWidget_data->sortItems(Qt::AscendingOrder);
        ui->lineEdit->clear();
    }
}

void CategoriesWidget::on_lineEdit_textChanged(const QString &arg1)
{
    mFiltrateListWidgetList();
}

void CategoriesWidget::on_buttonBox_accepted()
{
    auto selectItemList = ui->listWidget_data->selectedItems();
    for (auto &&item : selectItemList) {
        Global::openNoteModel->categoriesModel = item->data(Qt::UserRole).value<CategoriesModel *>();
    }

    emit categoriesChanged();
}
