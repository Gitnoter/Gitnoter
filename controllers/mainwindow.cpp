#include "ui_mainwindow.h"

#include "mainwindow.h"
#include "notelistsortpopupmenu.h"
#include "notelistcellwidget.h"
#include "tools.h"
#include "globals.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUi();

    Globals::noteModelList.init();
    Globals::categoryModelList.init();
    Globals::tagModelList.init();

    setNoteList();
    setCategoryList();
    setTagList();
    setOpenedNoteModel();
    updateUiContent();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUi()
{
    ui->treeWidget->expandAll();
    ui->treeWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->listWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEdit_noteSearch->addAction(QIcon(":/images/icon-search.png"), QLineEdit::LeadingPosition);
    ui->pushButton_sort->setMenu(new NoteListSortPopupMenu(ui->pushButton_sort, this));
    ui->splitter->setSizes(Globals::configModel->getSplitterSizes());
    ui->stackWidget_editor->setCurrentIndex((int) Globals::configModel->getOpenNoteUuid().isEmpty());
}

void MainWindow::updateUiContent()
{
    QList<NoteModel *> noteModelList = Globals::noteModelList.getList();

    int all = 0;
    int recent = 0;
    int unclassified = 0;
    int trash = 0;
    int category = 0;
    int tag = 0;
    for (auto &&item : noteModelList) {
        if (item->getIsDelete()) {
            trash += 1;
            continue;
        }
        else {
            all += 1;
        }

        if (item->getUpdateDate() > (QDateTime::currentSecsSinceEpoch() - 7 * 24 * 60 * 60)) {
            recent += 1;
        }

        if (item->getCategory().isEmpty()) {
            unclassified += 1;
        }
        else {
            category += 1;
        }

        if (item->getTagList().length()) {
            tag += 1;
        }
    }


    ui->treeWidget->topLevelItem(1)->setText(0, tr("全部笔记 (%1)").arg(all));
    ui->treeWidget->topLevelItem(2)->setText(0, tr("最近 7 天使用 (%1)").arg(recent));
    ui->treeWidget->topLevelItem(3)->setText(0, tr("未分类的笔记 (%1)").arg(unclassified));
    ui->treeWidget->topLevelItem(4)->setText(0, tr("回收站 (%1)").arg(trash));
    ui->treeWidget->topLevelItem(6)->setText(0, tr("归档 (%1)").arg(category));
    ui->treeWidget->topLevelItem(8)->setText(0, tr("标签 (%1)").arg(tag));

    QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(6);
    for (int i = 0; i < topLevelItem->childCount(); ++i) {
        QTreeWidgetItem *childItem = topLevelItem->child(i);
        CategoryModel *categoryModel = childItem->data(0, Qt::UserRole).value<CategoryModel *>();
        childItem->setText(0, tr("%1 (%2)").arg(categoryModel->getName()).arg(categoryModel->getCount()));
    }

    topLevelItem = ui->treeWidget->topLevelItem(8);
    for (int i = 0; i < topLevelItem->childCount(); ++i) {
        QTreeWidgetItem *childItem = topLevelItem->child(i);
        TagModel *tagModel = childItem->data(0, Qt::UserRole).value<TagModel *>();
        childItem->setText(0, tr("%1 (%2)").arg(tagModel->getName()).arg(tagModel->getCount()));
    }
}

void MainWindow::setNoteList()
{
    ui->listWidget->clear();
    int type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();
    for (auto &&noteModel : Globals::noteModelList.getList()) {
        if ((type == 1 && name == "all" && !noteModel->getIsDelete())
            || (type == 1 && name == "recent" && !noteModel->getIsDelete()
                && noteModel->getUpdateDate() > (QDateTime::currentSecsSinceEpoch() - 7 * 24 * 60 * 60))
            || (type == 1 && name == "unclassified" && !noteModel->getIsDelete() && noteModel->getCategory().isEmpty())
            || (type == 1 && name == "trash" && noteModel->getIsDelete())
            || (type == 2 && !noteModel->getIsDelete() && name == noteModel->getCategory())
            || (type == 2 && !noteModel->getIsDelete() && noteModel->getTagList().indexOf(name) != -1)) {

            QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->listWidget);
            listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(noteModel));
            ui->listWidget->addItem(listWidgetItem);
            NoteListCellWidget *noteListCellWidget = new NoteListCellWidget(noteModel, this);
            listWidgetItem->setSizeHint(noteListCellWidget->minimumSize());
            ui->listWidget->setItemWidget(listWidgetItem, noteListCellWidget);
        }
    }

    // TODO: fix layout bug
    ui->splitter->setSizes(Globals::configModel->getSplitterSizes());
}

void MainWindow::setCategoryList()
{
    QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(6);
    for (auto &&item : Globals::categoryModelList.getList()) {
        if (item->getName().isEmpty()) {
            continue;
        }
        QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();
        treeWidgetItem->setText(0, tr("%1 (%2)").arg(item->getName()).arg(item->getCount()));
        treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(item));
        topLevelItem->addChild(treeWidgetItem);
    }
}

void MainWindow::setTagList()
{
    QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(8);
    for (auto &&item : Globals::tagModelList.getList()) {
        if (item->getName().isEmpty()) {
            continue;
        }
        QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();
        treeWidgetItem->setText(0, tr("%1 (%2)").arg(item->getName()).arg(item->getCount()));
        treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(item));
        topLevelItem->addChild(treeWidgetItem);
    }
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    NoteModel *noteModel = item->data(Qt::UserRole).value<NoteModel *>();
    if (noteModel->getUuid() != mNoteModel->getUuid()) {
        Globals::configModel->setOpenNoteUuid(noteModel->getUuid());
        setOpenedNoteModel();
        emit noteListItemClicked(item);
    }
}

void MainWindow::on_splitter_splitterMoved(int, int)
{
    Globals::configModel->setSplitterSizes(ui->splitter->sizes());
}

void MainWindow::on_pushButton_noteAdd_clicked()
{
    mNoteModel = Globals::noteModelList.append(Globals::configModel->getSideSelectedName());
    Globals::configModel->setOpenNoteUuid(mNoteModel->getUuid());
    Globals::categoryModelList.append(mNoteModel->getCategory());
    ui->stackWidget_editor->setCurrentIndex(0);
    on_action_saveNote_triggered();
}

void MainWindow::on_action_saveNote_triggered()
{
    mNoteModel->saveNoteToLocal();
}

void MainWindow::on_action_deleteNote_triggered()
{
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *listWidgetItem = ui->listWidget->item(i);
        NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
        if (noteModel->getUuid() == mNoteModel->getUuid()) {
            delete listWidgetItem;
            break;
        }
    }

    Globals::categoryModelList.removeOne(mNoteModel->getCategory());
    Globals::tagModelList.removeList(mNoteModel->getTagList());
    Globals::noteModelList.deleteOne(mNoteModel);
    on_action_saveNote_triggered();
    Globals::configModel->setOpenNoteUuid("");
}

void MainWindow::setOpenedNoteModel()
{
    if (Globals::configModel->getOpenNoteUuid().isEmpty()) {
        ui->stackWidget_editor->setCurrentIndex(1);
        return;
    }
    mNoteModel = Globals::noteModelList.findByUuid(Globals::configModel->getOpenNoteUuid());
    ui->page_editor->init(mNoteModel);
    ui->stackWidget_editor->setCurrentIndex(0);
}

void MainWindow::on_pushButton_noteSubtract_clicked()
{
    on_action_deleteNote_triggered();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    int type = 0;
    QString name = "";
    if (item->data(0, Qt::UserRole).isNull()) {
        type = 1;
        int index = ui->treeWidget->indexOfTopLevelItem(item);
        switch (index) {
        case 1:name = "all";break;
        case 2:name = "recent";break;
        case 3:name = "unclassified";break;
        case 4:name = "trash";break;
        default:return;
        }
    }
    else {
        type = 2;
        if (item->parent() == ui->treeWidget->topLevelItem(6)) {
            name = item->data(0, Qt::UserRole).value<CategoryModel *>()->getName();
        }
        else {
            name = item->data(0, Qt::UserRole).value<TagModel *>()->getName();
        }
    }
    Globals::configModel->setSideSelected(type, name);
    setNoteList();
}
