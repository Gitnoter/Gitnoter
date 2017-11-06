#include "ui_mainwindow.h"

#include "mainwindow.h"
#include "notelistsortpopupmenu.h"
#include "notelistcellwidget.h"
#include "messagedialog.h"
#include "tools.h"
#include "globals.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Globals::noteModelList.init();
    Globals::categoryModelList.init();
    Globals::tagModelList.init();

    setNoteList();
    setCategoryList();
    setTagList();
    setOpenedNoteModel();
    setItemSelected();

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
    int type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();
    if (type == 1) {
        int index = 0;
        if (name == "all") index = 1;
        else if (name == "recent") index = 2;
        else if (name == "unclassified") index = 3;
        else if (name == "trash") index = 4;
        ui->treeWidget->topLevelItem(index)->setSelected(true);
    }
    else if(type == 2) {
        QTreeWidgetItem *treeWidgetItem = ui->treeWidget->topLevelItem(6);
        for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
            QTreeWidgetItem *childItem = treeWidgetItem->child(i);
            CategoryModel *categoryModel = childItem->data(0, Qt::UserRole).value<CategoryModel *>();
            if (categoryModel->getName() == name) {
                childItem->setSelected(true);
                break;
            }
        }
    }
    else if (type == 3) {
        QTreeWidgetItem *treeWidgetItem = ui->treeWidget->topLevelItem(8);
        for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
            QTreeWidgetItem *childItem = treeWidgetItem->child(i);
            TagModel *tagModel = childItem->data(0, Qt::UserRole).value<TagModel *>();
            if (tagModel->getName() == name) {
                childItem->setSelected(true);
                break;
            }
        }
    }

    for (int j = 0; j < ui->listWidget->count(); ++j) {
        QListWidgetItem *listWidgetItem = ui->listWidget->item(j);
        NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
        if (noteModel->getUuid() == Globals::configModel->getOpenNoteUuid()) {
            listWidgetItem->setSelected(true);
            break;
        }
    }

    // Check is selected list item
    bool hasFind = false;
    for (int j = 0; j < ui->listWidget->count(); ++j) {
        QListWidgetItem *listWidgetItem = ui->listWidget->item(j);
        NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
        if (noteModel->getUuid() == Globals::configModel->getOpenNoteUuid()) {
            listWidgetItem->setSelected(true);
            hasFind = true;
            break;
        }
    }
    if (!hasFind) {
        if (ui->listWidget->count() > 0) {
            QListWidgetItem *listWidgetItem = ui->listWidget->item(0);
            NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
            Globals::configModel->setOpenNoteUuid(noteModel->getUuid());
            listWidgetItem->setSelected(true);
        }
        else {
            Globals::configModel->setOpenNoteUuid("");
        }
    }
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
            || (type == 3 && !noteModel->getIsDelete() && noteModel->getTagList().indexOf(name) != -1)) {

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
        setOpenedNoteModel();
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
    setOpenedNoteModel();

    if (!mNoteModel->getIsDelete()) {
        Globals::noteModelList.deleteOne(mNoteModel);
        on_action_saveNote_triggered();
    }
    else {
        Globals::noteModelList.removeFolder(mNoteModel);
    }
}

void MainWindow::onNoteAdded()
{
    qDebug() << 1;
    if (mNoteModel->getIsDelete()) {
        mNoteModel->setIsDelete(0);
        mNoteModel->saveNoteDataToLocal();

        setNoteList();
        setItemSelected();
        setOpenedNoteModel();
        return;
    }

    mNoteModel = Globals::noteModelList.prepend(Globals::configModel->getSideSelectedType() == 2
                                                ? Globals::configModel->getSideSelectedName() : "");
    Globals::configModel->setOpenNoteUuid(mNoteModel->getUuid());
    Globals::categoryModelList.append(mNoteModel->getCategory());
    Globals::configModel->setOpenNoteUuid(mNoteModel->getUuid());
    ui->stackWidget_editor->setCurrentIndex(0);
    mNoteModel->saveNoteDataToLocal();

    if (QList<int>({1, 3}).indexOf(Globals::configModel->getSideSelectedType()) != -1) {
        Globals::configModel->setSideSelected(1, "all");
        ui->treeWidget->clearSelection();
        ui->treeWidget->topLevelItem(1)->setSelected(true);
    }

    setNoteList();
    setItemSelected();
    setOpenedNoteModel();
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
        if (item->parent() == ui->treeWidget->topLevelItem(6)) {
            type = 2;
            name = item->data(column, Qt::UserRole).value<CategoryModel *>()->getName();
        }
        else {
            type = 3;
            name = item->data(column, Qt::UserRole).value<TagModel *>()->getName();
        }
    }
    Globals::configModel->setSideSelected(type, name);
    setNoteList();
    setItemSelected();
    setOpenedNoteModel();
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
        QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(6);
        QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();
        treeWidgetItem->setText(0, category);
        treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(categoryModel));
        topLevelItem->addChild(treeWidgetItem);
        topLevelItem->sortChildren(0, Qt::AscendingOrder);
    }
}

void MainWindow::onCategoryDeleted(const QString &category)
{
    Globals::categoryModelList.removeAt(Globals::categoryModelList.indexOf(category));
    QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(6);
    for (int i = 0; i < topLevelItem->childCount(); ++i) {
        QTreeWidgetItem *childItem = topLevelItem->child(i);
        CategoryModel *categoryModel = childItem->data(0, Qt::UserRole).value<CategoryModel *>();
        if (categoryModel->getName() == category) {
            topLevelItem->removeChild(childItem);
        }
    }
}

void MainWindow::onTagAppend(const QString &tag)
{
    if (Globals::tagModelList.indexOf(tag) == -1) {
        TagModel *tagModel = Globals::tagModelList.append(tag);
        QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(8);
        QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();
        treeWidgetItem->setText(0, tag);
        treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(tagModel));
        topLevelItem->addChild(treeWidgetItem);
        topLevelItem->sortChildren(0, Qt::AscendingOrder);
    }
}

void MainWindow::onTagDeleted(const QString &tag)
{
    Globals::tagModelList.removeAt(Globals::tagModelList.indexOf(tag));
    QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(8);
    for (int i = 0; i < topLevelItem->childCount(); ++i) {
        QTreeWidgetItem *childItem = topLevelItem->child(i);
        TagModel *tagModel = childItem->data(0, Qt::UserRole).value<TagModel *>();
        if (tagModel->getName() == tag) {
            topLevelItem->removeChild(childItem);
        }
    }
}
