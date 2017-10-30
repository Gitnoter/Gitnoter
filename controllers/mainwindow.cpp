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

    Global::initRepoLocalDir();
    Global::initNoteModelList();
    Global::initCategoryModelList();
    Global::initTagModelList();
    Global::initGitManager();

    setNoteList();
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
}

void MainWindow::setNoteList()
{
    ui->listWidget->clear();
    for (auto &&noteMoldel : Global::noteModelList) {
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

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    emit noteListItemClicked(item);
}

void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    Global::configModel->setSplitterSizes(ui->splitter->sizes());
}

void MainWindow::on_pushButton_noteAdd_clicked()
{
    NoteModel *noteModel = new NoteModel();
    noteModel->setCategory(Global::configModel->getCategoryName());
    Global::setOpenNoteModel(noteModel);
    Global::addNoteModelToList(noteModel);
    on_action_saveNote_triggered();
}

void MainWindow::on_action_saveNote_triggered()
{
    Global::openNoteModel->writerLocal();
}
