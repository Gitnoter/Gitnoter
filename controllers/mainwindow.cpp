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
}

void MainWindow::setNoteList()
{
//    ui->listWidget->clear();
    for (auto &&noteMoldel : Global::noteModelList) {
        QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->listWidget);
        listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(noteMoldel));
        ui->listWidget->addItem(listWidgetItem);
        NoteListCellWidget *noteListCellWidget = new NoteListCellWidget(noteMoldel);
        listWidgetItem->setSizeHint(noteListCellWidget->minimumSize());
        ui->listWidget->setItemWidget(listWidgetItem, noteListCellWidget);
//        ui->listWidget->resize(noteListCellWidget->sizeHint());
        qDebug() << noteListCellWidget->minimumSize();
    }

}


