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
    setItemSelected();
    setGroupName();
    setOpenNote();

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
    const QString uuid = NoteModel::setItemSelected(ui->listWidget,
                                                    Globals::configModel->getOpenNoteUuid());
    Globals::configModel->setOpenNoteUuid(uuid);
}

void MainWindow::setNoteList()
{
    NoteModel::showListItems(ui->listWidget,
                             Globals::configModel->getSideSelectedType(),
                             Globals::configModel->getSideSelectedName());

    // TODO: fix layout bug
    ui->splitter->setSizes(Globals::configModel->getSplitterSizes());
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    NoteModel *noteModel = item->data(Qt::UserRole).value<NoteModel *>();
    if (noteModel->getUuid() != Globals::configModel->getOpenNoteUuid()) {
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
    if (Globals::configModel->getSideSelectedType() == Gitnoter::Trash) {
        if (Globals::configModel->getOpenNoteUuid().isEmpty()) {
            return;
        }

        MessageDialog *messageDialog = new MessageDialog(this);
        connect(messageDialog, SIGNAL(applyClicked()), this, SLOT(onNoteAdded()));
        NoteModel *noteModel = NoteModel::getNoteModelByUuid(ui->listWidget, Globals::configModel->getOpenNoteUuid());
        const QString category = noteModel->getCategory().isEmpty() ?
                                 ui->treeWidget->topLevelItem(1)->text(0) :
                                 noteModel->getCategory();
        messageDialog->openMessage(tr("笔记将被恢复到 %1\n\nTip: 长按添加按钮可恢复回收站内所有笔记哦~").arg(category),
                                   tr("恢复笔记提示"),
                                   tr("确定恢复"));
    }
    else {
        onNoteAdded();
    }
}

void MainWindow::on_action_saveNote_triggered()
{
    mNoteModel->saveNoteToLocal();
}

void MainWindow::onNoteDeleted()
{
    if (Globals::configModel->getSideSelectedType() == Gitnoter::Trash) {
        removeNote();
    }
    else {
        deleteNote();
    }
}

void MainWindow::onNoteAdded()
{
    if (Globals::configModel->getSideSelectedType() == Gitnoter::Trash) {
        restoreNote();
    }
    else {
        newNote();
    }
}

void MainWindow::setOpenNote()
{
    if (Globals::configModel->getOpenNoteUuid().isEmpty()) {
        ui->stackWidget_editor->setCurrentIndex(1);
        return;
    }

    NoteModel *noteModel = NoteModel::getNoteModelByUuid(ui->listWidget, Globals::configModel->getOpenNoteUuid());
    if (!noteModel) {
        ui->stackWidget_editor->setCurrentIndex(1);
        Globals::configModel->setOpenNoteUuid("");
        return;
    }
    ui->page_editor->init(noteModel, ui->treeWidget, ui->listWidget);
    ui->stackWidget_editor->setCurrentIndex(0);
}

void MainWindow::on_pushButton_noteSubtract_clicked()
{
    if (Globals::configModel->getSideSelectedType() == Gitnoter::Trash) {
        if (Globals::configModel->getOpenNoteUuid().isEmpty()) {
            return;
        }

        MessageDialog *messageDialog = new MessageDialog(this);
        connect(messageDialog, SIGNAL(applyClicked()), this, SLOT(onNoteDeleted()));
        messageDialog->openMessage(tr("删除后将无法恢复\n\nTip: 长按删除按钮可清空回收站哦~"), tr("删除笔记提示"), tr("确定删除"));
    }
    else {
        onNoteDeleted();
    }
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    if (item->data(0, Qt::UserRole).isNull()) {
        return;
    }

    Globals::configModel->setSideSelected(item->data(0, Qt::UserRole).value<GroupModel *>()->getType(), item->text(0));
    setNoteList();
    setItemSelected();
    setGroupName();
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
//    if (Globals::categoryModelList.indexOf(category) == -1) {
//        CategoryModel *categoryModel = Globals::categoryModelList.append(category);
//        addCategoryToTreeWidget(categoryModel);
//    }
}

void MainWindow::onGroupSubtracted(Gitnoter::GroupType type, const QString &name)
{
//    GroupModel::su
}

void MainWindow::onTagAppend(const QString &tag)
{
//    if (Globals::tagModelList.indexOf(tag) == -1) {
//        TagModel *tagModel = Globals::tagModelList.append(tag);
//        addTagToTreeWidget(tagModel);
//    }
}

void MainWindow::onTagDeleted(const QString &name, bool remove)
{
//    const QString _name = name.isEmpty() ? Globals::configModel->getSideSelectedName() : name;

//    if (_name.isEmpty()) {
//        return;
//    }

//    if (remove) {
//        Globals::tagModelList.removeAt(Globals::tagModelList.indexOf(_name));
//        QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(8);
//        for (int i = 0; i < topLevelItem->childCount(); ++i) {
//            QTreeWidgetItem *childItem = topLevelItem->child(i);
//            TagModel *tagModel = childItem->data(0, Qt::UserRole).value<TagModel *>();
//            if (tagModel->getName() == _name) {
//                topLevelItem->removeChild(childItem);
//            }
//        }
//    }
//    else {
//        Globals::tagModelList.removeOne(_name);
//    }
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
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();

    if (type == Gitnoter::Category || type == Gitnoter::Tag) {
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

void MainWindow::onGroupRemoved(Gitnoter::GroupType type, const QString &name)
{
    GroupModel::removeOne(ui->treeWidget, type, name);
    if (type == Gitnoter::Category) {
        Tools::writerFile(Globals::repoCategoryListPath, GroupModel::toString(ui->treeWidget, type));
    }
    else if (type == Gitnoter::Tag) {
        Tools::writerFile(Globals::repoTagListPath, GroupModel::toString(ui->treeWidget, type));
    }
}

void MainWindow::restoreNote()
{
    const QString uuid = Globals::configModel->getOpenNoteUuid();
    GroupModel::appendAny(GroupModel::getGroupModel(ui->treeWidget, Gitnoter::Trash), -1);
    NoteModel::appendOld(ui->listWidget, uuid);
    GroupModel::appendOne(ui->treeWidget, NoteModel::getNoteModelByUuid(ui->listWidget, uuid), 1);

    setNoteList();
    setItemSelected();
    setGroupName();
    setOpenNote();
}

void MainWindow::newNote()
{
    QString category = "";
    if (Globals::configModel->getOpenNoteUuid().isEmpty()) {
        Globals::configModel->setSideSelected(Gitnoter::All, ui->treeWidget->topLevelItem(1)->text(0));
    }
    else {
        NoteModel *noteModel = NoteModel::getNoteModelByUuid(ui->listWidget, Globals::configModel->getOpenNoteUuid());
        Globals::configModel->setSideSelected(Gitnoter::Category, noteModel->getCategory());
        category = noteModel->getCategory();
    }

    NoteModel *noteModel = NoteModel::appendNew(ui->listWidget, category);
    Globals::configModel->setOpenNoteUuid(noteModel->getUuid());
    GroupModel::appendOne(ui->treeWidget, noteModel, 1);
    noteModel->saveNoteToLocal();
    ui->stackWidget_editor->setCurrentIndex(0);

    setNoteList();
    setItemSelected();
    setGroupName();
    setOpenNote();
}

void MainWindow::removeNote()
{
    NoteModel *noteModel = NoteModel::getNoteModelByUuid(ui->listWidget, Globals::configModel->getOpenNoteUuid());
    if (noteModel) {
        NoteModel::removeOne(ui->listWidget, noteModel);
        GroupModel::appendAny(GroupModel::getGroupModel(ui->treeWidget, Gitnoter::Trash), -1);

        setNoteList();
        setItemSelected();
        setGroupName();
        setOpenNote();
    }
}

void MainWindow::deleteNote()
{
    NoteModel *noteModel = NoteModel::getNoteModelByUuid(ui->listWidget, Globals::configModel->getOpenNoteUuid());
    if (noteModel) {
        GroupModel::appendOne(ui->treeWidget, noteModel, -1);
        NoteModel::deleteOne(ui->listWidget, noteModel->getUuid());
        GroupModel::appendAny(GroupModel::getGroupModel(ui->treeWidget, Gitnoter::Trash), 1);

        setNoteList();
        setItemSelected();
        setGroupName();
        setOpenNote();
    }
}
