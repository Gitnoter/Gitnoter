#include "mainwindow.h"
#include "notelistsortpopupmenu.h"
#include "notelistcellwidget.h"

#include "tools.h"
#include "globals.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUi();

    Global::noteModelList.init();
    Global::categoryModelList.init();
    Global::tagModelList.init();

    setNoteList();
    setCategoryList();
    setTagList();
    setOpenedNoteModel();
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
    ui->splitter->setSizes(Global::configModel->getSplitterSizes());
    ui->stackWidget_editor->setCurrentIndex((int) Global::configModel->getUuidFromOpenNoteModel().isEmpty());
}

void MainWindow::setNoteList()
{
    ui->listWidget->clear();
    for (auto &&noteMoldel : Global::noteModelList.getList()) {
        QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->listWidget);
        listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(noteMoldel));
        ui->listWidget->addItem(listWidgetItem);
        NoteListCellWidget *noteListCellWidget = new NoteListCellWidget(noteMoldel, this);
        listWidgetItem->setSizeHint(noteListCellWidget->minimumSize());
        ui->listWidget->setItemWidget(listWidgetItem, noteListCellWidget);
    }

    // TODO: fix layout bug
    ui->splitter->setSizes(Global::configModel->getSplitterSizes());
}

void MainWindow::setCategoryList()
{
    QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(6);
    for (auto &&item : Global::categoryModelList.getList()) {
        QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();
        treeWidgetItem->setText(0, item->getName());
        treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(item));
        topLevelItem->addChild(treeWidgetItem);
    }
}

void MainWindow::setTagList()
{
    QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(8);
    for (auto &&item : Global::tagModelList.getList()) {
        QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();
        treeWidgetItem->setText(0, item->getName());
        treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(item));
        topLevelItem->addChild(treeWidgetItem);
    }
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    emit noteListItemClicked(item);
}

void MainWindow::on_splitter_splitterMoved(int, int)
{
    Global::configModel->setSplitterSizes(ui->splitter->sizes());
}

void MainWindow::on_pushButton_noteAdd_clicked()
{
    NoteModel *noteModel = Global::noteModelList.append(Global::configModel->getCategory());

    Global::configModel->setOpenNoteModel(noteModel);
    Global::categoryModelList.append(noteModel->getCategory());
    ui->stackWidget_editor->setCurrentIndex(0);
    on_action_saveNote_triggered();
}

void MainWindow::on_action_saveNote_triggered()
{
    Global::configModel->getOpenNoteModel()->saveNoteToLocal();
}

void MainWindow::on_action_deleteNote_triggered()
{
    Global::categoryModelList.removeOne(Global::configModel->getOpenNoteModel()->getCategory());
    Global::configModel->setOpenNoteModel("");
    Global::noteModelList.removeOne(Global::configModel->getOpenNoteModel());
//    Global::initCategoriesModelList();
//    Global::initTagsModelList();
//    mSetTableWidgetList();
//    mSetOpenedNoteModel();
}

void MainWindow::setOpenedNoteModel()
{
    if (Global::configModel->getUuidFromOpenNoteModel().isEmpty()) {
        ui->stackWidget_editor->setCurrentIndex(0);
        return;
    }
    NoteModel *noteModel = Global::noteModelList.findByUuid(Global::configModel->getUuidFromOpenNoteModel());
    if (noteModel == nullptr) {
        ui->stackWidget_editor->setCurrentIndex(0);
        return;
    }
    Global::configModel->setOpenNoteModel(noteModel);
    ui->page_editor->setMarkdownEditorText(Global::configModel->getOpenNoteModel()->getNoteText());
    ui->page_editor->setMarkdownPreviewText(Global::configModel->getOpenNoteModel()->getMarkdownHtml());
    ui->stackWidget_editor->setCurrentIndex(1);
}
