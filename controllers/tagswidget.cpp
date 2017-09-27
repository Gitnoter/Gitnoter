#include "tagswidget.h"
#include "mainwindow.h"
#include "ui_tagswidget.h"
#include "tools.h"
#include "globals.h"

#include <QDebug>

TagsWidget::TagsWidget(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::TagsWidget)
{
    ui->setupUi(this);

    ui->listWidget_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->pushButton_add->setHidden(true);

    mSetListWidgetList();
}

TagsWidget::~TagsWidget()
{
    delete ui;
}

void TagsWidget::mSetListWidgetList()
{
    ui->listWidget_data->clear();
    for (int i = 0; i < Global::tagsModelList.length(); ++i) {
        ui->listWidget_data->addItem(Global::tagsModelList[i]->getName());
        ui->listWidget_data->item(i)->setData(Qt::UserRole, QVariant::fromValue(Global::tagsModelList[i]));
        for (auto &&item : *Global::openNoteModel->tagsModelList) {
            if (item->getName() == Global::tagsModelList[i]->getName()) {
                ui->listWidget_data->setItemSelected(ui->listWidget_data->item(i), true);
            }
        }
    }
}

void TagsWidget::mFiltrateListWidgetList()
{
    int showCount = 0;
    QString text = ui->lineEdit->displayText();
    for (int i = 0; i < Global::tagsModelList.length(); ++i) {
        int index = Global::tagsModelList[i]->getName().indexOf(text, 0, Qt::CaseInsensitive);
        QListWidgetItem *listWidgetItem = ui->listWidget_data->item(i);

        bool isHidden = text.isEmpty() ? false : index == -1;
        listWidgetItem->setHidden(isHidden);
        if (!isHidden) showCount += 1;
    }

    ui->pushButton_add->setHidden(showCount != 0);
}

void TagsWidget::on_pushButton_add_clicked()
{
    if (!ui->lineEdit->displayText().isEmpty()) {
        Global::tagsModelList.append(new TagsModel(ui->lineEdit->displayText()));
        Global::addTagsModelList();
        mSetListWidgetList();
        ui->listWidget_data->sortItems(Qt::AscendingOrder);
        ui->lineEdit->clear();
    }
}

void TagsWidget::on_lineEdit_textChanged(const QString &arg1)
{
    mFiltrateListWidgetList();
}

void TagsWidget::on_buttonBox_accepted()
{
    auto selectItemList = ui->listWidget_data->selectedItems();
    Global::openNoteModel->tagsModelList->clear();
    for (auto &&item : selectItemList) {
        Global::openNoteModel->tagsModelList->append(item->data(Qt::UserRole).value<TagsModel *>());
    }

    emit tagsChanged();
}

