#include "ui_mainwindow.h"

#include "mainwindow.h"
#include "notelistsortpopupmenu.h"
#include "notelistcellwidget.h"
#include "messagedialog.h"
#include "tools.h"
#include "globals.h"
#include "groupmodel.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{    ui->setupUi(this);

    QList<NoteModel *> noteModelList = NoteModel::init(ui->listWidget);
    GroupModel::init(ui->treeWidget, noteModelList);

    setNoteList();
    setOpenNote();
    setItemSelected();
    setGroupName();

    setupUi();
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

    connect(ui->page_editor, SIGNAL(noteModelChanged(NoteModel *)), this, SLOT(onNoteModelChanged(NoteModel *)));
    connect(ui->page_editor, SIGNAL(tagAppend(const QString &)), this, SLOT(onTagAppend(const QString &)));
    connect(ui->page_editor, SIGNAL(tagDeleted(const QString &)), this, SLOT(onTagDeleted(const QString &)));
    connect(ui->page_editor, SIGNAL(categoryAppend(const QString &)), this, SLOT(onCategoryAppend(const QString &)));
}

void MainWindow::setItemSelected()
{
    GroupModel::setItemSelected(ui->treeWidget, 
                                Globals::configModel->getSideSelectedType(), 
                                Globals::configModel->getSideSelectedName());

    // Check is selected list item
    Globals::configModel->setOpenNoteUuid(NoteModel::setItemSelected(ui->listWidget,
                                                                     Globals::configModel->getOpenNoteUuid()));
}

void MainWindow::setNoteList()
{
    ui->listWidget->clear();
    GroupModel::GroupType type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();
    NoteModel::showListItems(ui->listWidget, type, name);
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
//        treeWidgetItem->setText(0, tr("%1 (%2)").arg(item->getName()).arg(item->getCount()));
        treeWidgetItem->setText(0, item->getName());
        treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(item));
        topLevelItem->addChild(treeWidgetItem);
    }
    topLevelItem->sortChildren(0, Qt::AscendingOrder);
}

void MainWindow::setTagList()
{
    QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(8);
    for (auto &&item : Globals::tagModelList.getList()) {
        if (item->getName().isEmpty()) {
            continue;
        }
        QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();
//        treeWidgetItem->setText(0, tr("%1 (%2)").arg(item->getName()).arg(item->getCount()));
        treeWidgetItem->setText(0, item->getName());
        treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(item));
        topLevelItem->addChild(treeWidgetItem);
    }
    topLevelItem->sortChildren(0, Qt::AscendingOrder);
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    NoteModel *noteModel = item->data(Qt::UserRole).value<NoteModel *>();
    if (noteModel->getUuid() != mNoteModel->getUuid()) {
        Globals::configModel->setOpenNoteUuid(noteModel->getUuid());
        setOpenNote();
    }
}

void MainWindow::on_splitter_splitterMoved(int, int)
{
    Globals::configModel->setSplitterSizes(ui->splitter->sizes());
}

void MainWindow::on_pushButton_noteAdd_clicked()
{
    if (mNoteModel->getIsDelete()) {
        MessageDialog *messageDialog = new MessageDialog(this);
        connect(messageDialog, SIGNAL(applyClicked()), this, SLOT(onNoteAdded()));
        const QString category = mNoteModel->getCategory().isEmpty() ? tr("所有笔记") : mNoteModel->getCategory();
        messageDialog->openMessage(tr("笔记将被恢复到 %1\n\nTip: 长按添加按钮可恢复回收站内所有笔记哦~").arg(category),
                                   tr("恢复笔记提示"), tr("确定恢复"));
        return;
    }
    onNoteAdded();
}

void MainWindow::on_action_saveNote_triggered()
{
    mNoteModel->saveNoteToLocal();
}

void MainWindow::onNoteDeleted()
{
    if (ui->listWidget->selectedItems().length() > 0) {
        delete ui->listWidget->selectedItems().at(0);
    }

    Globals::categoryModelList.removeOne(mNoteModel->getCategory());
    Globals::tagModelList.removeList(mNoteModel->getTagList());
    Globals::configModel->setOpenNoteUuid("");
    setItemSelected();
    setOpenNote();

    if (!mNoteModel->getIsDelete()) {
        NoteModel::deleteOne(ui->listWidget, mNoteModel);
        on_action_saveNote_triggered();
    }
    else {
        NoteModel::removeOne(ui->listWidget, mNoteModel);
    }
}

void MainWindow::onNoteAdded()
{
    if (mNoteModel->getIsDelete()) {
        mNoteModel->setIsDelete(0);
        mNoteModel->saveNoteDataToLocal();

        setNoteList();
        setItemSelected();
        setOpenNote();
        return;
    }

    NoteModel::append(ui->listWidget, Globals::configModel->getSideSelectedType() == 2
                                       ? Globals::configModel->getSideSelectedName()
                                       : "");
    Globals::configModel->setOpenNoteUuid(mNoteModel->getUuid());
    Globals::categoryModelList.append(mNoteModel->getCategory());
    Globals::configModel->setOpenNoteUuid(mNoteModel->getUuid());
    ui->stackWidget_editor->setCurrentIndex(0);
    mNoteModel->saveNoteDataToLocal();

    if (QList<int>({1, 3}).indexOf(Globals::configModel->getSideSelectedType()) != -1) {
        Globals::configModel->setSideSelected(GroupModel::All, ui->treeWidget->topLevelItem(1)->text(0));
        ui->treeWidget->clearSelection();
        ui->treeWidget->topLevelItem(1)->setSelected(true);
    }

    setNoteList();
    setItemSelected();
    setOpenNote();
}

void MainWindow::setOpenNote()
{
    if (Globals::configModel->getOpenNoteUuid().isEmpty()) {
        ui->stackWidget_editor->setCurrentIndex(1);
        return;
    }

    ui->page_editor->init(NoteModel::findNoteModelByUuid(ui->listWidget, Globals::configModel->getOpenNoteUuid()));
    ui->stackWidget_editor->setCurrentIndex(0);
}

void MainWindow::on_pushButton_noteSubtract_clicked()
{
    if (mNoteModel->getIsDelete()) {
        MessageDialog *messageDialog = new MessageDialog(this);
        connect(messageDialog, SIGNAL(applyClicked()), this, SLOT(onNoteDeleted()));
        messageDialog->openMessage(tr("删除后将无法恢复\n\nTip: 长按删除按钮可清空回收站哦~"), tr("删除笔记提示"), tr("确定删除"));
        return;
    }
    onNoteDeleted();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    if (item->data(0, Qt::UserRole).isNull()) {
        return;
    }

    Globals::configModel->setSideSelected(item->data(0, Qt::UserRole).value<GroupModel *>()->getType(), item->text(0));
    setNoteList();
    setGroupName();
    setItemSelected();
    setOpenNote();
}

void MainWindow::onNoteModelChanged(NoteModel *noteModel)
{
    for (int j = 0; j < ui->listWidget->count(); ++j) {
        QListWidgetItem *listWidgetItem = ui->listWidget->item(j);
        NoteModel *itemNoteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
        if (itemNoteModel == noteModel) {
            ((NoteListCellWidget *) ui->listWidget->itemWidget(listWidgetItem))->reloadData();
            break;
        }
    }
}

void MainWindow::onCategoryAppend(const QString &category)
{
    if (Globals::categoryModelList.indexOf(category) == -1) {
        CategoryModel *categoryModel = Globals::categoryModelList.append(category);
//        addCategoryToTreeWidget(categoryModel);
    }
}

void MainWindow::onGroupSubtracted(GroupModel::GroupType type, const QString &name)
{
//    GroupModel::su
}

void MainWindow::onTagAppend(const QString &tag)
{
    if (Globals::tagModelList.indexOf(tag) == -1) {
        TagModel *tagModel = Globals::tagModelList.append(tag);
//        addTagToTreeWidget(tagModel);
    }
}

void MainWindow::onTagDeleted(const QString &name, bool remove)
{
    const QString _name = name.isEmpty() ? Globals::configModel->getSideSelectedName() : name;

    if (_name.isEmpty()) {
        return;
    }

    if (remove) {
        Globals::tagModelList.removeAt(Globals::tagModelList.indexOf(_name));
        QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(8);
        for (int i = 0; i < topLevelItem->childCount(); ++i) {
            QTreeWidgetItem *childItem = topLevelItem->child(i);
            TagModel *tagModel = childItem->data(0, Qt::UserRole).value<TagModel *>();
            if (tagModel->getName() == _name) {
                topLevelItem->removeChild(childItem);
            }
        }
    }
    else {
        Globals::tagModelList.removeOne(_name);
    }
}

void MainWindow::on_pushButton_add_clicked()
{
    int type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();

    if (type == 2) {
        onCategoryAppend(name);
    }
    else if (type == 3) {

    }
}

void MainWindow::on_pushButton_subtract_clicked()
{
    GroupModel::GroupType type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();

    if (type == GroupModel::Category || type == GroupModel::Tag) {
        onGroupRemoved(type, name);
    }
}

void MainWindow::setGroupName()
{
    QList<QTreeWidgetItem *> treeWidgetItemList = ui->treeWidget->selectedItems();
    if (!treeWidgetItemList.length()) {
        return;
    }
    GroupModel *groupModel = treeWidgetItemList[0]->data(0, Qt::UserRole).value<GroupModel *>();
    ui->label_groupName->setText(tr("%1 (%2)").arg(groupModel->getName()).arg(groupModel->getCount()));
}

void MainWindow::onGroupRemoved(GroupModel::GroupType type, const QString &name)
{
    GroupModel::removeOne(ui->treeWidget, type, name);
    if (type == GroupModel::Category) {
        Tools::writerFile(Globals::repoCategoryListPath, GroupModel::toString(ui->treeWidget, type));
    }
    else if (type == GroupModel::Tag) {
        Tools::writerFile(Globals::repoTagListPath, GroupModel::toString(ui->treeWidget, type));
    }
}

