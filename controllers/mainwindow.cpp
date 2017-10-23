#include "mainwindow.h"
#include "notelistsortpopupmenu.h"
#include "notelistcellwidget.h"

#include "previewpage.h"
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
    Global::initCategoriesModelList();
    Global::initTagsModelList();
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
