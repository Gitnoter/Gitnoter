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

    QList<NoteModel *> noteModelList = NoteModel::init(ui->noteModelList);
    GroupModel::init(ui->groupModelList, noteModelList);

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
    ui->groupModelList->expandAll();
    ui->groupModelList->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->noteModelList->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEdit_noteSearch->addAction(QIcon(":/images/icon-search.png"), QLineEdit::LeadingPosition);
    ui->pushButton_sort->setMenu(new NoteListSortPopupMenu(ui->pushButton_sort, this));

    ui->splitter->setSizes(Globals::configModel->getSplitterSizes());
    ui->stackWidget_editor->setCurrentIndex((int) Globals::configModel->getOpenNoteUuid().isEmpty());

    connect(ui->page_editor, SIGNAL(noteModelChanged(NoteModel *)), this, SLOT(onNoteModelChanged(NoteModel *)));
    connect(ui->page_editor, SIGNAL(groupModelListDeleted(Gitnoter::GroupType, const QString &)),
            this, SLOT(onGroupModelListDeleted(Gitnoter::GroupType, const QString &)));
    connect(ui->page_editor, SIGNAL(groupModelListAppend(Gitnoter::GroupType, const QString &)),
            this, SLOT(onGroupModelListAppend(Gitnoter::GroupType, const QString &)));
}

void MainWindow::setItemSelected()
{
    GroupModel::setItemSelected(ui->groupModelList,
                                Globals::configModel->getSideSelectedType(), 
                                Globals::configModel->getSideSelectedName());

    // Check is selected list item
    const QString uuid = NoteModel::setItemSelected(ui->noteModelList,
                                                    Globals::configModel->getOpenNoteUuid());
    Globals::configModel->setOpenNoteUuid(uuid);
}

void MainWindow::setNoteList()
{
    NoteModel::showListItems(ui->noteModelList,
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
        NoteModel *noteModel = NoteModel::getNoteModelByUuid(ui->noteModelList, Globals::configModel->getOpenNoteUuid());
        const QString category = noteModel->getCategory().isEmpty() ?
                                 ui->groupModelList->topLevelItem(1)->text(0) :
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
//    mNoteModel->saveNoteToLocal();
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

    NoteModel *noteModel = NoteModel::getNoteModelByUuid(ui->noteModelList, Globals::configModel->getOpenNoteUuid());
    if (!noteModel) {
        ui->stackWidget_editor->setCurrentIndex(1);
        Globals::configModel->setOpenNoteUuid("");
        return;
    }
    ui->page_editor->init(noteModel, ui->groupModelList, ui->noteModelList);
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
    for (int j = 0; j < ui->noteModelList->count(); ++j) {
        QListWidgetItem *listWidgetItem = ui->noteModelList->item(j);
        NoteModel *itemNoteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
        if (itemNoteModel == noteModel) {
            ((NoteListCellWidget *) ui->noteModelList->itemWidget(listWidgetItem))->reloadData();
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

void MainWindow::onGroupModelListAppend(Gitnoter::GroupType type, const QString &text)
{
    if (GroupModel::has(GroupModel::getGroupModelList(ui->groupModelList, type), type, text)) {
        GroupModel::appendAny(GroupModel::getGroupModel(ui->groupModelList, type, text), 1);
    }
    else {
        GroupModel::append(ui->groupModelList, new GroupModel(type, text, 1));
    }
}

void MainWindow::onGroupModelListDeleted(Gitnoter::GroupType type, const QString &text)
{
    if (GroupModel::has(GroupModel::getGroupModelList(ui->groupModelList, type), type, text)) {
        GroupModel::appendAny(GroupModel::getGroupModel(ui->groupModelList, type, text), -1);
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
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();

    if (type == Gitnoter::Category || type == Gitnoter::Tag) {
        onGroupRemoved(type, name);
    }
}

void MainWindow::setGroupName()
{
    QList<QTreeWidgetItem *> treeWidgetItemList = ui->groupModelList->selectedItems();
    if (!treeWidgetItemList.length()) {
        return;
    }
    GroupModel *groupModel = treeWidgetItemList[0]->data(0, Qt::UserRole).value<GroupModel *>();
    ui->label_groupName->setText(tr("%1 (%2)").arg(groupModel->getName()).arg(groupModel->getCount()));
}

void MainWindow::onGroupRemoved(Gitnoter::GroupType type, const QString &name)
{
    GroupModel::removeOne(ui->groupModelList, type, name);
    if (type == Gitnoter::Category) {
        Tools::writerFile(Globals::repoCategoryListPath, GroupModel::toString(ui->groupModelList, type));
    }
    else if (type == Gitnoter::Tag) {
        Tools::writerFile(Globals::repoTagListPath, GroupModel::toString(ui->groupModelList, type));
    }
}

void MainWindow::restoreNote()
{
    const QString uuid = Globals::configModel->getOpenNoteUuid();
    GroupModel::appendAny(GroupModel::getGroupModel(ui->groupModelList, Gitnoter::Trash), -1);
    NoteModel::appendOld(ui->noteModelList, uuid);
    GroupModel::appendOne(ui->groupModelList, NoteModel::getNoteModelByUuid(ui->noteModelList, uuid), 1);

    setNoteList();
    setItemSelected();
    setGroupName();
    setOpenNote();
}

void MainWindow::newNote()
{
    QString category = "";
    if (Globals::configModel->getOpenNoteUuid().isEmpty()) {
        Globals::configModel->setSideSelected(Gitnoter::All, ui->groupModelList->topLevelItem(1)->text(0));
    }
    else {
        NoteModel *noteModel = NoteModel::getNoteModelByUuid(ui->noteModelList, Globals::configModel->getOpenNoteUuid());
        Globals::configModel->setSideSelected(Gitnoter::Category, noteModel->getCategory());
        category = noteModel->getCategory();
    }

    NoteModel *noteModel = NoteModel::appendNew(ui->noteModelList, category);
    Globals::configModel->setOpenNoteUuid(noteModel->getUuid());
    GroupModel::appendOne(ui->groupModelList, noteModel, 1);
    noteModel->saveNoteToLocal();
    ui->stackWidget_editor->setCurrentIndex(0);

    setNoteList();
    setItemSelected();
    setGroupName();
    setOpenNote();
}

void MainWindow::removeNote()
{
    NoteModel *noteModel = NoteModel::getNoteModelByUuid(ui->noteModelList, Globals::configModel->getOpenNoteUuid());
    if (noteModel) {
        NoteModel::removeOne(ui->noteModelList, noteModel);
        GroupModel::appendAny(GroupModel::getGroupModel(ui->groupModelList, Gitnoter::Trash), -1);

        setNoteList();
        setItemSelected();
        setGroupName();
        setOpenNote();
    }
}

void MainWindow::deleteNote()
{
    NoteModel *noteModel = NoteModel::getNoteModelByUuid(ui->noteModelList, Globals::configModel->getOpenNoteUuid());
    if (noteModel) {
        GroupModel::appendOne(ui->groupModelList, noteModel, -1);
        NoteModel::deleteOne(ui->noteModelList, noteModel->getUuid());
        GroupModel::appendAny(GroupModel::getGroupModel(ui->groupModelList, Gitnoter::Trash), 1);

        setNoteList();
        setItemSelected();
        setGroupName();
        setOpenNote();
    }
}

void MainWindow::modifyTextAllNote(Gitnoter::GroupType type, const QString &oldText, const QString &newText)
{
    for (int i = 0; i < ui->noteModelList->count(); ++i) {
        QListWidgetItem *listWidgetItem = ui->noteModelList->item(i);
        NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();

        if (Gitnoter::Tag == type) {
            QStringList tagList = noteModel->getTagList();
            if (tagList.removeOne(oldText)) {
                if (!newText.isEmpty()) {
                    tagList.append(newText);
                }
                noteModel->saveNoteToLocal();

                qDebug() << noteModel->getTagList();
            }
        }
        else if (Gitnoter::Category == type) {
            if (noteModel->getCategory() == oldText) {
                if (!newText.isEmpty()) {
                    noteModel->setCategory(newText);
                    noteModel->saveNoteToLocal();
                }
            }
        }
    }
}

void MainWindow::removeTextAllNote(Gitnoter::GroupType type, const QString &oldText)
{
    for (int i = 0; i < ui->noteModelList->count(); ++i) {
        QListWidgetItem *listWidgetItem = ui->noteModelList->item(i);
        NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();

        if (Gitnoter::Tag == type) {
            QStringList tagList = noteModel->getTagList();
            tagList.removeOne(oldText);
            noteModel->saveNoteToLocal();
        }
        else if (Gitnoter::Category == type) {
            if (noteModel->getCategory() == oldText) {
                GroupModel::appendOne(ui->groupModelList, noteModel, -1);
                noteModel->setCategory("");
                noteModel->setIsDelete(1);
                GroupModel::appendAny(GroupModel::getGroupModel(ui->groupModelList, Gitnoter::Trash), 1);
                noteModel->saveNoteToLocal();
            }
        }
    }
}
