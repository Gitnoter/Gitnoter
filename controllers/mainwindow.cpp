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
{
    ui->setupUi(this);
    init();
    setupUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUi()
{
    ui->groupTreeWidget->expandAll();
    ui->groupTreeWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->noteListWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEdit_noteSearch->addAction(QIcon(":/images/icon-search.png"), QLineEdit::LeadingPosition);

    NoteListSortPopupMenu *noteListSortPopupMenu = new NoteListSortPopupMenu(ui->pushButton_sort, this);
    ui->pushButton_sort->setMenu(noteListSortPopupMenu);
    connect(noteListSortPopupMenu, SIGNAL(actionTriggered()), ui->noteListWidget, SLOT(onActionClicked()));

    ui->splitter->setSizes(Globals::configModel->getSplitterSizes());
    ui->stackWidget_editor->setCurrentIndex((int) Globals::configModel->getOpenNoteUuid().isEmpty());
}

void MainWindow::on_noteListWidget_itemClicked(QListWidgetItem *item)
{
    NoteModel *noteModel = item->data(Qt::UserRole).value<NoteModel *>();
    if (noteModel->getUuid() != Globals::configModel->getOpenNoteUuid()) {
        Globals::configModel->setOpenNoteUuid(noteModel->getUuid());
        markdownEditorWidget()->init(noteModel, this);
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
        connect(messageDialog, SIGNAL(applyClicked()), this, SLOT(restoreNoteListWidget()));
        NoteModel *noteModel = ui->noteListWidget->getNoteModel(Globals::configModel->getOpenNoteUuid());
        const QString category = noteModel->getCategory().isEmpty() ?
                                 ui->groupTreeWidget->topLevelItem(1)->text(0) :
                                 noteModel->getCategory();
        messageDialog->openMessage(tr("笔记将被恢复到 %1\n\nTip: 长按添加按钮可恢复回收站内所有笔记哦~").arg(category),
                                   tr("恢复笔记提示"),
                                   tr("确定恢复"));
    }
    else {
        appendNoteListWidgetItem();
    }
}

void MainWindow::on_action_saveNote_triggered()
{
//    mNoteModel->saveNoteToLocal();
}

void MainWindow::on_pushButton_noteSubtract_clicked()
{
    if (Globals::configModel->getSideSelectedType() == Gitnoter::Trash) {
        if (Globals::configModel->getOpenNoteUuid().isEmpty()) {
            return;
        }

        MessageDialog *messageDialog = new MessageDialog(this);
        connect(messageDialog, SIGNAL(applyClicked()), this, SLOT(trashNoteListWidget()));
        messageDialog->openMessage(tr("删除后将无法恢复\n\nTip: 长按删除按钮可清空回收站哦~"), tr("删除笔记提示"), tr("确定删除"));
    }
    else {
        removeNoteListWidgetItem();
    }
}

void MainWindow::on_groupTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    if (item->data(0, Qt::UserRole).isNull()) {
        return;
    }

    ui->lineEdit_noteSearch->clear();
    Globals::configModel->setSideSelected(groupTreeWidget()->getGroupModel(item));
    ui->noteListWidget->setListWidget();
    ui->markdownEditorWidget->init(Globals::configModel->getOpenNoteUuid(), this);
    setNoteListWidgetTitle();
}

void MainWindow::on_pushButton_add_clicked()
{

}

void MainWindow::on_pushButton_subtract_clicked()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();

    if (type == Gitnoter::Category || type == Gitnoter::Tag) {
        ui->groupTreeWidget->remove(type, name);
    }
}

void MainWindow::setNoteListWidgetTitle()
{
    GroupModel *groupModel = ui->groupTreeWidget->selectedGroupModel();
    if (groupModel) {
        ui->label_groupName->setText(tr("%1 (%2)").arg(groupModel->getName()).arg(groupModel->getCount()));
    }
}

void MainWindow::restoreNoteListWidget()
{
    noteListWidget()->restore(Globals::configModel->getOpenNoteUuid());
}

void MainWindow::appendNoteListWidgetItem()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    QString category = Globals::configModel->getSideSelectedName();

    if (Gitnoter::Category != type) {
        category = "";
    }

    noteListWidget()->append(category);
}

void MainWindow::removeNoteListWidgetItem()
{
    noteListWidget()->remove(Globals::configModel->getOpenNoteUuid());
}

void MainWindow::trashNoteListWidget()
{
    noteListWidget()->trash(Globals::configModel->getOpenNoteUuid());
}

void MainWindow::modifyTextAllNote(Gitnoter::GroupType type, const QString &oldText, const QString &newText)
{
//    for (int i = 0; i < ui->noteListWidget->count(); ++i) {
//        QListWidgetItem *listWidgetItem = ui->noteListWidget->item(i);
//        NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();

//        if (Gitnoter::Tag == type) {
//            QStringList tagList = noteModel->getTagList();
//            if (tagList.removeOne(oldText)) {
//                if (!newText.isEmpty()) {
//                    tagList.append(newText);
//                }
//                noteModel->saveNoteToLocal();

//                qDebug() << noteModel->getTagList();
//            }
//        }
//        else if (Gitnoter::Category == type) {
//            if (noteModel->getCategory() == oldText) {
//                if (!newText.isEmpty()) {
//                    noteModel->setCategory(newText);
//                    noteModel->saveNoteToLocal();
//                }
//            }
//        }
//    }
}

void MainWindow::removeTextAllNote(Gitnoter::GroupType type, const QString &oldText)
{
//    for (int i = 0; i < ui->noteListWidget->count(); ++i) {
//        QListWidgetItem *listWidgetItem = ui->noteListWidget->item(i);
//        NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();

//        if (Gitnoter::Tag == type) {
//            QStringList tagList = noteModel->getTagList();
//            tagList.removeOne(oldText);
//            noteModel->saveNoteToLocal();
//        }
//        else if (Gitnoter::Category == type) {
//            if (noteModel->getCategory() == oldText) {
//                ui->groupTreeWidget->appendOne(noteModel, -1);
//                noteModel->setCategory("");
//                noteModel->setIsDelete(1);
//                ui->groupTreeWidget->appendAny(ui->groupTreeWidget->getGroupModel(Gitnoter::Trash), 1);
//                noteModel->saveNoteToLocal();
//            }
//        }
//    }
}

GroupTreeWidget *MainWindow::groupTreeWidget()
{
    return ui->groupTreeWidget;
}

NoteListWidget *MainWindow::noteListWidget()
{
    return ui->noteListWidget;
}

MarkdownEditorWidget *MainWindow::markdownEditorWidget()
{
    return ui->markdownEditorWidget;
}

void MainWindow::init()
{
    ui->noteListWidget->init(this);
    ui->groupTreeWidget->init(ui->noteListWidget->getNoteModelList(), this);
    ui->markdownEditorWidget->init(Globals::configModel->getOpenNoteUuid(), this);

    setNoteListWidgetTitle();
}

void MainWindow::removeGroupTreeWidgetItem()
{

}

void MainWindow::appendGroupTreeWidgetItem()
{

}

void MainWindow::searchNoteListWidget()
{

}

void MainWindow::sortNoteListWidget()
{

}

QStackedWidget *MainWindow::stackedWidget()
{
    return ui->stackWidget_editor;
}

QSplitter *MainWindow::splitter()
{
    return ui->splitter;
}

void MainWindow::on_lineEdit_noteSearch_textChanged(const QString &arg1)
{
    noteListWidget()->search(arg1);
}
