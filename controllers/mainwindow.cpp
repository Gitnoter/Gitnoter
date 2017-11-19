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
    connect(noteListSortPopupMenu, SIGNAL(actionTriggered()), ui->noteListWidget, SLOT(onActionTriggered()));

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
        connect(messageDialog, SIGNAL(applyClicked()), this, SLOT(restoreNote()));
        NoteModel *noteModel = ui->noteListWidget->getNoteModel(Globals::configModel->getOpenNoteUuid());
        const QString category = noteModel->getCategory().isEmpty() ?
                                 ui->groupTreeWidget->topLevelItem(1)->text(0) :
                                 noteModel->getCategory();
        messageDialog->openMessage(tr("笔记将被恢复到 %1\n\nTip: 长按添加按钮可恢复回收站内所有笔记哦~").arg(category),
                                   tr("恢复笔记提示"),
                                   tr("确定恢复"));
    }
    else {
        appendNote();
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
        connect(messageDialog, SIGNAL(applyClicked()), this, SLOT(removeNote()));
        messageDialog->openMessage(tr("删除后将无法恢复\n\nTip: 长按删除按钮可清空回收站哦~"), tr("删除笔记提示"), tr("确定删除"));
    }
    else {
        trashNote();
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
    setNoteListTitle();
}

void MainWindow::on_pushButton_add_clicked()
{
    appendGroup();
}

void MainWindow::on_pushButton_subtract_clicked()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    if (Gitnoter::Category <= type) {
        MessageDialog *messageDialog = new MessageDialog(this);
        connect(messageDialog, SIGNAL(applyClicked()), this, SLOT(removeGroup()));
        messageDialog->openMessage(tr("笔记本删除后, 笔记本内的笔记将会移动到回收站~ \n\nTip: 还没想好要说些什么o(╯□╰)o"), tr("删除笔记本提示"), tr("确定删除"));
    }
}

void MainWindow::setNoteListTitle()
{
    GroupModel *groupModel = ui->groupTreeWidget->selectedGroupModel();
    if (groupModel) {
        ui->label_groupName->setText(tr("%1 (%2)").arg(groupModel->getName()).arg(groupModel->getCount()));
    }
}

void MainWindow::restoreNote()
{
    noteListWidget()->restore(Globals::configModel->getOpenNoteUuid());

    noteListWidget()->setListWidget();
    setNoteListTitle();
    markdownEditorWidget()->init(Globals::configModel->getOpenNoteUuid(), this);
}

void MainWindow::appendNote()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    QString category = Globals::configModel->getSideSelectedName();

    if (Gitnoter::Category != type) {
        category = "";
    }

    NoteModel *noteModel = noteListWidget()->append(category);
    Globals::configModel->setOpenNoteUuid(noteModel->getUuid());
    noteListWidget()->setListWidget();
    groupTreeWidget()->setItemSelected();
    markdownEditorWidget()->init(noteModel, this);
}

void MainWindow::removeNote()
{
    noteListWidget()->remove(Globals::configModel->getOpenNoteUuid());
    noteListWidget()->setListWidget();
    setNoteListTitle();
    markdownEditorWidget()->init(Globals::configModel->getOpenNoteUuid(), this);
}

void MainWindow::trashNote()
{
    noteListWidget()->trash(Globals::configModel->getOpenNoteUuid());
    noteListWidget()->setListWidget();
    setNoteListTitle();
    markdownEditorWidget()->init(Globals::configModel->getOpenNoteUuid(), this);
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

    setNoteListTitle();
}

void MainWindow::removeGroup()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();

    if (Gitnoter::Category <= type) {
        Globals::configModel->setSideSelected(Gitnoter::All, groupTreeWidget()->getGroupModel(Gitnoter::All)->getName());

        ui->groupTreeWidget->remove(type, name);
        noteListWidget()->setListWidget();
        setNoteListTitle();
        markdownEditorWidget()->init(Globals::configModel->getOpenNoteUuid(), this);
    }
}

void MainWindow::appendGroup()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    if (Gitnoter::Tag == type) {
        QString categoriesName = "";
        for (int i = 0; i < 100; ++i) {
            categoriesName = tr("新建笔记本%1").arg(i == 0 ? "" : QString::number(i));
            if (groupTreeWidget()->append(Gitnoter::Tag, categoriesName)) {
                groupTreeWidget()->editItem(groupTreeWidget()->getTreeWidgetItem(Gitnoter::Tag, categoriesName));
                Globals::configModel->setSideSelected(Gitnoter::Tag, categoriesName);
                noteListWidget()->setListWidget();
                groupTreeWidget()->setItemSelected();
                setNoteListTitle();
                markdownEditorWidget()->init(Globals::configModel->getOpenNoteUuid(), this);
                break;
            }
        }
    }
    else {

    }
}

void MainWindow::searchNote(const QString &text)
{
    noteListWidget()->search(text);
}

void MainWindow::sortNote()
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
    searchNote(arg1);
}
