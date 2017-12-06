#include "ui_mainwindow.h"
#include "ui_menubar.h"

#include "mainwindow.h"
#include "notelistsortpopupmenu.h"
#include "notelistcellwidget.h"
#include "messagedialog.h"
#include "tools.h"
#include "globals.h"
#include "groupmodel.h"
#include "version.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        mMenuBar(new MenuBar(this)),
        mAutoSyncRepoTimer(new QTimer(this)),
        mAutoLockTimer(new QTimer(this))
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
    mWindowList.append(this);
    setMenuBar(mMenuBar->menuBar());
    ui->groupTreeWidget->expandAll();
    ui->groupTreeWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->noteListWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEdit_noteSearch->addAction(QIcon(":/images/icon-search.png"), QLineEdit::LeadingPosition);

    NoteListSortPopupMenu *noteListSortPopupMenu = new NoteListSortPopupMenu(ui->pushButton_sort, this);
    ui->pushButton_sort->setMenu(noteListSortPopupMenu);
    connect(noteListSortPopupMenu, SIGNAL(actionTriggered()), ui->noteListWidget, SLOT(onActionTriggered()));

    ui->splitter->setSizes(Globals::configModel->getMainWindowSplitterSizes());
    ui->stackWidget_editor->setCurrentIndex((int) Globals::configModel->openMainWindowNoteUuid().isEmpty());

    setGeometry(Globals::configModel->getMainWindowRect());
    if (Globals::configModel->getMainWindowFullScreen()) {
        showFullScreen();
    }

    updateAutoLockTimer();
    updateAutoSyncRepoTimer();

    showSidebar(Globals::configModel->getSidebarWidget());
    showListBar(Globals::configModel->getListBarWidget());

    connect(ui->noteListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(newWindow(QListWidgetItem *)));

    connect(mMenuBar->getActionNewNote(), SIGNAL(triggered()), this, SLOT(appendNote()));
    connect(mMenuBar->getActionNewCategories(), SIGNAL(triggered()), this, SLOT(appendGroup()));
    connect(mMenuBar->getActionNewTags(), SIGNAL(triggered()), this, SLOT(appendGroup()));
    connect(mMenuBar->getActionReloadNotes(), SIGNAL(triggered()), this, SLOT(init()));
    connect(mMenuBar->getActionPreferences(), SIGNAL(triggered()), this, SLOT(openSettingWidget()));
    connect(mMenuBar->getActionDeleteNote(), SIGNAL(triggered()), this, SLOT(trashNote()));
    connect(mMenuBar->getActionGroupSearch(), SIGNAL(triggered()), this, SLOT(setSearchFocus()));
    connect(mMenuBar->getActionSidebar(), SIGNAL(triggered(bool)), this, SLOT(showSidebar(bool)));
    connect(mMenuBar->getActionListbar(), SIGNAL(triggered(bool)), this, SLOT(showListBar(bool)));
    connect(mMenuBar->getActionSynchNote(), SIGNAL(triggered()), this, SLOT(syncRepo()));

    connect(mMenuBar->getActionEnterFullScreen(), SIGNAL(triggered()), this, SLOT(enterFullScreen()));
    connect(mMenuBar->getActionFullScreenEditMode(), SIGNAL(triggered()), this, SLOT(fullScreenEditMode()));

    connect(mMenuBar->getActionMixWindow(), SIGNAL(triggered()), this, SLOT(showMinimized()));
    connect(mMenuBar->getActionResizeWindow(), SIGNAL(triggered()), this, SLOT(showMaximized()));

    connect(mMenuBar->getActionNewWindow(), SIGNAL(triggered()), this, SLOT(newWindow()));
    connect(mMenuBar->getActionShowLastWindow(), SIGNAL(triggered()), this, SLOT(showLastWindow()));
    connect(mMenuBar->getActionShowNextWindow(), SIGNAL(triggered()), this, SLOT(showNextWindow()));
    connect(mMenuBar->getActionCloseWindow(), SIGNAL(triggered()), this, SLOT(closeWindow()));
    connect(mMenuBar->getActionCloseAllWindow(), SIGNAL(triggered()), this, SLOT(closeAllWindow()));
    connect(mMenuBar->getActionLockWindow(), SIGNAL(triggered()), this, SLOT(lockWindow()));
    connect(mMenuBar->getActionPreposeWindow(), SIGNAL(triggered()), this, SLOT(preposeWindow()));

    connect(mMenuBar->getActionGuide(), SIGNAL(triggered()), this, SLOT(guide()));
    connect(mMenuBar->getActionHistory(), SIGNAL(triggered()), this, SLOT(historyChange()));
    connect(mMenuBar->getActionMarkdownLanguage(), SIGNAL(triggered()), this, SLOT(markdownLanguage()));
    connect(mMenuBar->getActionFeedback(), SIGNAL(triggered()), this, SLOT(feedback()));
    connect(mMenuBar->getActionIssue(), SIGNAL(triggered()), this, SLOT(issue()));
    connect(mMenuBar->getActionAbout(), SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::on_noteListWidget_itemClicked(QListWidgetItem *item)
{
    NoteModel *noteModel = item->data(Qt::UserRole).value<NoteModel *>();
    if (noteModel->getUuid() != Globals::configModel->openMainWindowNoteUuid()) {
        Globals::configModel->setOpenMainWindowNoteUuid(noteModel->getUuid());
        markdownEditorWidget()->init(noteModel, this);
    }
}

void MainWindow::on_splitter_splitterMoved(int, int)
{
    Globals::configModel->setMainWindowSplitterSizes(ui->splitter->sizes());
}

void MainWindow::on_pushButton_noteAdd_clicked()
{
    if (Globals::configModel->getSideSelectedType() == Gitnoter::Trash) {
        if (Globals::configModel->openMainWindowNoteUuid().isEmpty()) {
            return;
        }

        MessageDialog *messageDialog = new MessageDialog(this);
        connect(messageDialog, SIGNAL(applyClicked()), this, SLOT(restoreNote()));
        NoteModel *noteModel = ui->noteListWidget->getNoteModel(Globals::configModel->openMainWindowNoteUuid());
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
        if (Globals::configModel->openMainWindowNoteUuid().isEmpty()) {
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
    updateView(Gitnoter::NoteListWidget);
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
        if (!groupModel->getCount()) setWindowTitle(groupModel->getName());
    }
}

void MainWindow::restoreNote()
{
    noteListWidget()->restore(Globals::configModel->openMainWindowNoteUuid());
    updateView(Gitnoter::NoteListWidget);
}

void MainWindow::appendNote()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    QString category = Globals::configModel->getSideSelectedName();

    if (Gitnoter::Category != type) {
        category = "";
    }

    NoteModel *noteModel = noteListWidget()->append(category);
    Globals::configModel->setOpenMainWindowNoteUuid(noteModel->getUuid());
    noteListWidget()->setListWidget();
    groupTreeWidget()->setItemSelected();
    markdownEditorWidget()->init(noteModel, this);
}

void MainWindow::removeNote()
{
    noteListWidget()->remove(Globals::configModel->openMainWindowNoteUuid());
    updateView(Gitnoter::NoteListWidget);
}

void MainWindow::trashNote()
{
    noteListWidget()->trash(Globals::configModel->openMainWindowNoteUuid());
    updateView(Gitnoter::NoteListWidget);
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
    updateView(Gitnoter::NoteListTitle | Gitnoter::MarkdownEditorWidget);
}

void MainWindow::removeGroup()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();

    if (Gitnoter::Category <= type) {
        Globals::configModel->setSideSelected(groupTreeWidget()->getGroupModel(Gitnoter::All));
        ui->groupTreeWidget->remove(type, name);
        updateView(Gitnoter::NoteListWidget);
    }
}

void MainWindow::appendGroup()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    QString name = "";
    for (int i = 0; i < 100; ++i) {
        name = ((Gitnoter::Tag == type) ? tr("新建标签%1") : tr("新建笔记本%1")).arg(i == 0 ? "" : QString::number(i));
        if (groupTreeWidget()->append(type, name)) {
            groupTreeWidget()->editItem(groupTreeWidget()->getTreeWidgetItem(type, name));
            Globals::configModel->setSideSelected(type, name);
            updateView(Gitnoter::GroupTreeWidget);
            break;
        }
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

void MainWindow::updateView(Gitnoter::UpdateViewFlags flags)
{
    if (flags.testFlag(Gitnoter::GroupTreeWidget)) {
        groupTreeWidget()->setItemSelected();
    }

    if (flags.testFlag(Gitnoter::NoteListWidget)) {
        noteListWidget()->setListWidget();
    }

    if (flags.testFlag(Gitnoter::MarkdownEditorWidget)) {
        markdownEditorWidget()->init(Globals::configModel->openMainWindowNoteUuid(), this);
    }

    if (flags.testFlag(Gitnoter::NoteListTitle)) {
        setNoteListTitle();
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange || event->type() == QEvent::ActivationChange) {
        if (windowState().testFlag(Qt::WindowFullScreen)) {
            Globals::configModel->setMainWindowFullScreen(true);
        }
        else if ((windowState() == Qt::WindowNoState || windowState() == Qt::WindowMaximized) && !isFullScreen()) {
            Globals::configModel->setMainWindowFullScreen(false);
        }
    }
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    QMainWindow::moveEvent(event);
    Globals::configModel->setMainWindowRect(geometry());
}

MenuBar *MainWindow::menuBar()
{
    return mMenuBar;
}

void MainWindow::openSettingWidget()
{

}

void MainWindow::setSearchFocus()
{
    ui->lineEdit_noteSearch->setFocus();
}

void MainWindow::updateAutoSyncRepoTimer()
{
    qDebug() << "updateAutoSyncRepoTimer";
    int autoSyncRepo = Globals::configModel->getAutoSyncRepoTime();
    if (autoSyncRepo) {
        connect(mAutoSyncRepoTimer, SIGNAL(timeout()), this, SLOT(syncRepo()));
        mAutoSyncRepoTimer->start(autoSyncRepo);
    }
    else if (mAutoSyncRepoTimer->isActive()){
        mAutoSyncRepoTimer->stop();
    }
}

void MainWindow::updateAutoLockTimer()
{
    int autoLockTime = Globals::configModel->getAutoLockTime();
    if (autoLockTime) {
        connect(mAutoLockTimer, SIGNAL(timeout()), this, SLOT(lockWindow()));
        mAutoLockTimer->start(autoLockTime);
    }
    else if (mAutoLockTimer->isActive()){
        mAutoLockTimer->stop();
    }
}

void MainWindow::syncRepo()
{
    qDebug() << "syncRepo" << __func__;
    Gitnoter::RepoStatus status = Globals::configModel->getLocalRepoStatus();
    if (Gitnoter::NoneRepo == status) {
        return;
    }

//            Globals::gitManager->commitA();
//            Globals::gitManager->pull();
//            Globals::gitManager->commitA();
//
//            if (Gitnoter::RemoteRepo == status) {
//                Globals::gitManager->push();
//            }
}
void MainWindow::lockWindow()
{
    qDebug() << "lockWindow" << __func__;
//            if (mAutoLockTimer->isActive()){
//                mAutoLockTimer->stop();
//            }
//            close();
//            (new LockDialog())->show();
}

void MainWindow::showSidebar(bool clicked)
{
    mMenuBar->getActionSidebar()->setChecked(clicked);

    ui->widget_sidebar->setVisible(clicked);
    Globals::configModel->setSidebarWidget(clicked);
}

void MainWindow::showListBar(bool clicked)
{
    mMenuBar->getActionListbar()->setChecked(clicked);

    ui->widget_listBar->setVisible(clicked);
    Globals::configModel->setListBarWidget(clicked);
}

void MainWindow::enterFullScreen()
{
    (isActiveWindow() && isFullScreen()) ? showNormal() : showFullScreen();
}

void MainWindow::fullScreenEditMode()
{
    if (isActiveWindow() && isFullScreen()) {
        bool sidebar = Globals::configModel->getSidebarWidget();
        bool listBar = Globals::configModel->getListBarWidget();

        ui->widget_listBar->setVisible(sidebar);
        ui->widget_sidebar->setVisible(listBar);

        showNormal();
    }
    else {
        ui->widget_listBar->setVisible(false);
        ui->widget_sidebar->setVisible(false);

        showFullScreen();
    }
}

void MainWindow::newWindow(QListWidgetItem *)
{
    const QString uuid = Globals::configModel->openMainWindowNoteUuid();
    if (uuid.isEmpty()) {
        return;
    }

    for (auto &&window : mWindowList) {
        if (MarkdownEditorWidget *widget
                = reinterpret_cast<MarkdownEditorWidget *>(qobject_cast<MarkdownEditorWidget *>(window))) {
            if (widget->noteModel()->getUuid() == uuid) {
                widget->raise();
                window->activateWindow();
                return;
            }
        }
    }

    MarkdownEditorWidget *markdownEditorWidget = new MarkdownEditorWidget;
    mWindowList.append(markdownEditorWidget);
    markdownEditorWidget->init(uuid, this);
    markdownEditorWidget->show();

    mMenuBar->setWindowMenu(markdownEditorWidget);
}

void MainWindow::showLastWindow()
{
    if (mWindowList.length() < 2) {
        return;
    }

    int nowActiveWindowIndex = -1;
    for (int i = 0; i < mWindowList.length(); ++i) {
        if (mWindowList[i]->isActiveWindow()) {
            nowActiveWindowIndex = i;
            break;
        }
    }

    if (nowActiveWindowIndex < 1) {
        return;
    }

    mWindowList[nowActiveWindowIndex - 1]->raise();
    mWindowList[nowActiveWindowIndex - 1]->activateWindow();

    mMenuBar->setWindowMenu();
}

void MainWindow::showNextWindow()
{
    if (mWindowList.length() < 2) {
        return;
    }

    int nowActiveWindowIndex = -1;
    for (int i = 0; i < mWindowList.length(); ++i) {
        if (mWindowList[i]->isActiveWindow()) {
            nowActiveWindowIndex = i;
            break;
        }
    }

    if (nowActiveWindowIndex == -1 || nowActiveWindowIndex == mWindowList.length() - 1) {
        return;
    }

    mWindowList[nowActiveWindowIndex + 1]->raise();
    mWindowList[nowActiveWindowIndex + 1]->activateWindow();

    mMenuBar->setWindowMenu();
}

void MainWindow::closeWindow()
{
    for (auto &&widget : mWindowList) {
        if (widget->isActiveWindow()) {
            mWindowList.removeOne(widget);
            widget->close();
            break;
        }
    }

    Globals::configModel->setOpenWindowListNoteUuid(mWindowList);

    mMenuBar->setWindowMenu();
}

void MainWindow::closeAllWindow()
{
    for (auto &&widget : mWindowList) {
        if (MarkdownEditorWidget *markdownEditorWidget
                = reinterpret_cast<MarkdownEditorWidget *>(qobject_cast<MarkdownEditorWidget *>(widget))) {
            markdownEditorWidget->close();
            mWindowList.removeOne(widget);
        }
    }

    raise();
    activateWindow();

    Globals::configModel->clearOpenWindowListNoteUuid();

    mMenuBar->setWindowMenu();
}

void MainWindow::preposeWindow()
{
    if (mWindowList.length() < 2) {
        return;
    }

    QWidget *nowActiveWindow = nullptr;
    for (auto &&widget : mWindowList) {
        if (widget->isActiveWindow()) {
            nowActiveWindow = widget;
            break;
        }
        else {
            widget->raise();
            widget->activateWindow();
        }
    }

    if (nowActiveWindow) {
        nowActiveWindow->raise();
        nowActiveWindow->activateWindow();
    }
}

void MainWindow::guide()
{
    QDesktopServices::openUrl(QUrl(Globals::guideUrl));
}

void MainWindow::historyChange()
{
    QDesktopServices::openUrl(QUrl(Globals::historyChangeUrl));
}

void MainWindow::markdownLanguage()
{
    QDesktopServices::openUrl(QUrl(Globals::markdownLanguageUrl));
}

void MainWindow::feedback()
{
    const QString body = "\n\n" + VER_FILEDESCRIPTION_STR + "\n" + QSysInfo::prettyProductName();
    QDesktopServices::openUrl(
            QUrl(QString("mailto:?to=%1&subject=Gitnoter for Mac: Feedback&body=%2").arg(
                    Globals::emailToUser, body), QUrl::TolerantMode));
}

void MainWindow::issue()
{
    QDesktopServices::openUrl(QUrl(Globals::issueUrl));
}

void MainWindow::about()
{

}
