#include "mainwindow.h"
#include "categorieslistcell.h"
#include "tagslistcell.h"
#include "lockdialog.h"

#include "previewpage.h"
#include "tools.h"
#include "globals.h"

#include "ui_mainwindow.h"

#include <QWebChannel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mInitAppMenu();
    settingDialog = new SettingDialog(this);
    aboutDialog = new AboutDialog(this);
    autoSyncRepoTimer = new QTimer(this);
    autoLockTimer = new QTimer(this);

    connect(settingDialog, SIGNAL(autoSyncRepoTimeChanged()), this, SLOT(onAutoSyncRepoTimerStart()));
    connect(settingDialog, SIGNAL(autoLockTimeChanged()), this, SLOT(onAutoLockTimerStart()));
    connect(settingDialog, SIGNAL(editorFontChanged()), this, SLOT(onEditorFontChange()));

    ui->listWidget_categories->setAcceptDrops(false);   // 不接受拖放 ui编辑器编辑无用代码, 可能是bug
    ui->lineEdit_searchNote->setHidden(true);           // 隐藏暂时不用的组件

    // 屏蔽选中时的边框颜色
    ui->lineEdit_title->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->listWidget_categories->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->listWidget_tags->setAttribute(Qt::WA_MacShowFocusRect, 0);

    connect(ui->tableWidget_list->horizontalHeader(), &QHeaderView::sortIndicatorChanged,
            this, &MainWindow::onHeaderViewSortIndicatorChanged);

    Global::initRepoLocalDir();
    Global::initNoteModelList();
    Global::initCategoriesModelList();
    Global::initTagsModelList();
    Global::initGitManager();

    mSetTableWidgetList();
    mSetOpenedNoteModel(true);
    onEditorFontChange();

    onAutoSyncRepoTimerStart();
    onAutoLockTimerStart();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::textChanged()
{
    Global::openNoteModel->contentModel->setTitle(ui->lineEdit_title->displayText().isEmpty()
                                           ? ui->lineEdit_title->placeholderText()
                                           : ui->lineEdit_title->displayText());
    Global::openNoteModel->contentModel->setBody(ui->plainTextEdit_editor->toPlainText());
    Global::openNoteModel->contentModel->setUpdateDate(0);
    m_content.setText(Global::openNoteModel->getDisplayNote());
}

void MainWindow::on_pushButton_categories_clicked()
{
    categoriesWidget = new CategoriesWidget(this);
    categoriesWidget->open();

    connect(categoriesWidget, SIGNAL(categoriesChanged()), this, SLOT(onCategoriesChange()));
}

void MainWindow::mSetOpenedNoteModel(bool initEditor)
{
    if (Global::configModel->getOpenNotesUuid().isEmpty()) {
        if (Tools::rowShowCount(ui->tableWidget_list) > 0) {
            for (int i = 0; i < ui->tableWidget_list->rowCount(); ++i) {
                if (!ui->tableWidget_list->isRowHidden(i)) {
                    auto *noteModel = ui->tableWidget_list->item(i, 0)->data(Qt::UserRole).value<NoteModel *>();
                    Global::configModel->setOpenNotesUuid(noteModel->contentModel->getUuid());
                    mSetOpenedNoteModel(true);
                    return;
                }
            }
        }
        Global::setOpenNoteModel(new NoteModel(Tools::readerFile(":/marked/default.md")));
    }
    else {
        Global::setOpenNoteModel(Global::getNoteModelByUuid(Global::configModel->getOpenNotesUuid()));
    }

    // 初始化编辑器
    if (initEditor) {
        ui->webEngineView_preview->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        ui->webEngineView_preview->setContextMenuPolicy(Qt::NoContextMenu);

        PreviewPage *page = new PreviewPage(this);
        ui->webEngineView_preview->setPage(page);

        QWebChannel *channel = new QWebChannel(this);
        channel->registerObject(QStringLiteral("content"), &m_content);
        page->setWebChannel(channel);

        ui->webEngineView_preview->setUrl(QUrl("qrc:/marked/index.html"));
    }

    ui->lineEdit_title->setText(Global::openNoteModel->contentModel->getTitle());
    ui->plainTextEdit_editor->setPlainText(Global::openNoteModel->contentModel->getBody());
    m_content.setText(Global::openNoteModel->getDisplayNote());

    // 监听编辑器中文本是否有更改
    connect(ui->plainTextEdit_editor, &QPlainTextEdit::textChanged, this, &MainWindow::textChanged);
    connect(ui->lineEdit_title, &QLineEdit::textChanged, this, &MainWindow::textChanged);
    mSetEditorModified(false);

    ui->pushButton_changeTags->setText(Global::openNoteModel->getTagsString());
    ui->pushButton_categories->setText(Global::openNoteModel->categoriesModel->getName());
}

void MainWindow::mSetTableWidgetList()
{
    ui->tableWidget_list->clearContents();

    ui->tableWidget_list->setRowCount(Global::noteModelList.length());
    for (int i = 0; i < Global::noteModelList.length(); ++i) {
        auto *tableWidgetItem0 = new QTableWidgetItem(Global::noteModelList[i]->contentModel->getTitle());
        tableWidgetItem0->setData(Qt::UserRole, QVariant::fromValue(Global::noteModelList[i]));
        auto *tableWidgetItem1 = new QTableWidgetItem(
                Tools::timestampToDateTime(Global::noteModelList[i]->contentModel->getUpdateDate()));
        tableWidgetItem1->setData(Qt::UserRole, QVariant::fromValue(Global::noteModelList[i]));
        ui->tableWidget_list->setItem(i, 0, tableWidgetItem0);
        ui->tableWidget_list->setItem(i, 1, tableWidgetItem1);
    }

    int sidebarSortKey = Global::configModel->getSidebarSortKey();
    Qt::SortOrder sidebarSortValue = Global::configModel->getSidebarSortValue() == "DESC" ? Qt::DescendingOrder
                                                                                   : Qt::AscendingOrder;
    ui->tableWidget_list->horizontalHeader()->sortIndicatorChanged(sidebarSortKey, sidebarSortValue);
    ui->tableWidget_list->horizontalHeader()->setSortIndicator(sidebarSortKey, sidebarSortValue);

    mFiltrateTableWidgetList();
}

void MainWindow::mFiltrateTableWidgetList()
{
    int firstShowRowIndex = 0;
    bool hasOpenNoteInList = false;
    QString text = ui->lineEdit_searchNoteList->displayText();

    for (int row = 0; row < ui->tableWidget_list->rowCount(); ++row) {
        QTableWidgetItem *tableWidgetItem = ui->tableWidget_list->item(row, 0);
        NoteModel *noteModel = tableWidgetItem->data(Qt::UserRole).value<NoteModel *>();
        bool hidden = true;

        if (Global::configModel->getIsSelectedClasses() == 1) {
            if (noteModel->categoriesModel->getName() == Global::configModel->getCategoriesName()) {
                hidden = false;
                if (!text.isEmpty()) {
                    hidden = noteModel->contentModel->getTitle().indexOf(text, 0, Qt::CaseInsensitive) == -1;
                }
            }
        }
        else if (Global::configModel->getIsSelectedClasses() == 2) {
            for (auto &&tagsModel : *noteModel->tagsModelList) {
                if (tagsModel->getName() == Global::configModel->getTagsName()) {
                    hidden = false;
                    if (!text.isEmpty()) {
                        hidden = noteModel->contentModel->getTitle().indexOf(text, 0, Qt::CaseInsensitive) == -1;
                    }
                    break;
                }
            }
        }

        ui->tableWidget_list->setRowHidden(row, hidden);

        if (!hidden) {
            firstShowRowIndex = row;
            if (!hasOpenNoteInList && noteModel->contentModel->getUuid() == Global::configModel->getOpenNotesUuid()) {
                ui->tableWidget_list->selectRow(row);
                hasOpenNoteInList = true;
            }
        }
    }

    if (!hasOpenNoteInList && firstShowRowIndex > 0) {
        QTableWidgetItem *tableWidgetItem = ui->tableWidget_list->item(firstShowRowIndex, 0);
        NoteModel *noteModel = tableWidgetItem->data(Qt::UserRole).value<NoteModel *>();
        Global::configModel->setOpenNotesUuid(noteModel->contentModel->getUuid());
        ui->tableWidget_list->selectRow(firstShowRowIndex);
    }
    else {
        Global::configModel->setOpenNotesUuid("");
    }
}

void MainWindow::on_tableWidget_list_itemClicked(QTableWidgetItem *item)
{
    QString uuid = item->data(Qt::UserRole).value<NoteModel *>()->contentModel->getUuid();
    if (Global::configModel->getOpenNotesUuid() == uuid) {
        return;
    }
    Global::configModel->setOpenNotesUuid(uuid);
    mSetOpenedNoteModel();
}

void MainWindow::on_tableWidget_list_doubleClicked(const QModelIndex &index)
{

}

void MainWindow::on_action_saveNote_triggered()
{
    if (!isEditorModified()) {
        return;
    }

    Tools::writerFile(Global::openNoteModel->contentModel->getFilePath(), Global::openNoteModel->getNote());
    Global::initCategoriesModelList();
    Global::initTagsModelList();
    mSetTableWidgetList();
}

void MainWindow::on_action_newNote_triggered()
{
    NoteModel *noteModel = new NoteModel(new CategoriesModel(Global::configModel->getCategoriesName()));
    Global::setOpenNoteModel(noteModel);
    mSetOpenedNoteModel();
    mSetEditorModified(true);
    on_action_saveNote_triggered();
}

bool MainWindow::isEditorModified()
{
    return ui->lineEdit_title->isModified() || ui->plainTextEdit_editor->document()->isModified();
}

void MainWindow::mSetEditorModified(bool m)
{
    ui->lineEdit_title->setModified(m);
    ui->plainTextEdit_editor->document()->setModified(m);
}

void MainWindow::on_pushButton_deleteNote_clicked()
{
    on_action_deleteNote_triggered();
}

void MainWindow::on_action_deleteNote_triggered()
{
    if (!Global::openNoteModel->contentModel->getUuid().isEmpty()) {
        QFile::remove(Global::openNoteModel->contentModel->getFilePath());
    }
    Global::configModel->setOpenNotesUuid("");
    Global::noteModelList.removeOne(Global::openNoteModel);
    Global::initCategoriesModelList();
    Global::initTagsModelList();

    mSetTableWidgetList();
    mSetOpenedNoteModel();
}

void MainWindow::onHeaderViewSortIndicatorChanged(int logicalIndex, Qt::SortOrder order)
{
    Global::configModel->setSidebarSortKey(logicalIndex);
    Global::configModel->setSidebarSortValue(order == Qt::DescendingOrder ? "DESC" : "ASC");

    qDebug() << "on_headerView_sortIndicatorChanged";
}

void MainWindow::onCategoriesChange()
{
    mSetOpenedNoteModel();
    mSetEditorModified(true);
    on_action_saveNote_triggered();
}

void MainWindow::on_pushButton_folder_clicked()
{
    mSelectedSidebarButtonByName(1);
    mSetCategoriesListWidgetList();
}

void MainWindow::on_listWidget_categories_itemClicked(QListWidgetItem *item)
{
    QWidget *widget = ui->listWidget_categories->itemWidget(item)->findChild<QWidget *>("widget");

    for (int i = 0; i < Global::categoriesModelList.length(); ++i) {
        QListWidgetItem *listWidgetItem = ui->listWidget_categories->item(i);
        QWidget *widget2 = ui->listWidget_categories->itemWidget(listWidgetItem)->findChild<QWidget *>("widget");
        if (widget == widget2) {
            Tools::changeWidgetBorder(widget2, "#7EBFF5", 2);
        }
        else {
            Tools::changeWidgetBorder(widget2, "#DFDFE0", 1);
        }
    }
    ui->listWidget_categories->setItemSelected(item, true);
}

void MainWindow::on_pushButton_notes_clicked()
{
    mSelectedSidebarButtonByName(0);
}

QMenu *MainWindow::createListWidgetCategoriesMenu()
{
    QMenu *popMenu = new QMenu(this);

    QAction *action_createCategories;
    QAction *action_renameCategories;
    QAction *action_removeCategories;
    QAction *action_nameSort;
    QAction *action_countSort;
    QAction *action_timeSort;

    action_createCategories = new QAction("新建文件夹", this);
    action_renameCategories = new QAction("重命名笔记本", this);
    action_removeCategories = new QAction("删除笔记本", this);
    action_nameSort = new QAction("按笔记本名称排序", this);
    action_countSort = new QAction("按笔记数量排序", this);
    action_timeSort = new QAction("按笔记更新时间排序", this);

    action_createCategories->setObjectName(QStringLiteral("action_createCategories"));
    action_renameCategories->setObjectName(QStringLiteral("action_renameCategories"));
    action_removeCategories->setObjectName(QStringLiteral("action_removeCategories"));
    action_nameSort->setObjectName(QStringLiteral("action_nameSort"));
    action_countSort->setObjectName(QStringLiteral("action_countSort"));
    action_timeSort->setObjectName(QStringLiteral("action_timeSort"));

    popMenu->addAction(action_createCategories);
    popMenu->addAction(action_renameCategories);
    popMenu->addAction(action_removeCategories);
    popMenu->addSeparator();
    popMenu->addAction(action_nameSort);
    popMenu->addAction(action_countSort);
    popMenu->addAction(action_timeSort);

    connect(action_createCategories, &QAction::triggered, this, &MainWindow::on_pushButton_addCategories_clicked);
    connect(action_renameCategories, &QAction::triggered, this, &MainWindow::onActionRenameCategoriesTriggered);
    connect(action_removeCategories, &QAction::triggered, this, &MainWindow::on_pushButton_removeCategories_clicked);
    connect(action_nameSort, &QAction::triggered, this, &MainWindow::onActionNameSortTriggered);
    connect(action_countSort, &QAction::triggered, this, &MainWindow::onActionCountSortTriggered);
    connect(action_timeSort, &QAction::triggered, this, &MainWindow::onActionTimeSortTriggered);

    return popMenu;
}

void MainWindow::onActionNameSortTriggered()
{
    qDebug() << "on_action_nameSort_triggered";
}

void MainWindow::onActionCountSortTriggered()
{

}

void MainWindow::onActionTimeSortTriggered()
{
    qDebug() << "onActionTimeSortTriggered";
}

void MainWindow::onLineEditNameCategoriesEditingFinished()
{
    int index = ui->listWidget_categories->selectionModel()->selectedIndexes()[0].row();
    QListWidgetItem *listWidgetItem = ui->listWidget_categories->item(index);
    CategoriesModel *categoriesModel = listWidgetItem->data(Qt::UserRole).value<CategoriesModel *>();
    QWidget *widget = ui->listWidget_categories->itemWidget(ui->listWidget_categories->selectedItems()[0]);
    QLineEdit *lineEdit_name = widget->findChild<QWidget *>("widget")->findChild<QLineEdit *>("lineEdit_name");
    QString name = lineEdit_name->displayText();

    if (!name.isEmpty() && !Global::hasInCategoriesModelList(name)) {
        if (categoriesModel->getName() != name) {
            Global::renameCategoriesModelListInName(categoriesModel->getName(), name);
            categoriesModel->setName(name);
        }
    }

    lineEdit_name->setText(categoriesModel->getName());
    lineEdit_name->setEnabled(false);
    setCategoriesListEnabled(true);
}

void MainWindow::on_pushButton_addCategories_clicked()
{
    QString categoriesName;
    QString lineEditSearchCategories = ui->lineEdit_searchCategories->displayText();

    for (int i = 0; i < 100; ++i) {
        categoriesName = lineEditSearchCategories.isEmpty()
                         ? tr("新建笔记本%1").arg(i == 0 ? "" : QString::number(i))
                         : lineEditSearchCategories;

        if (Global::addCategoriesModelList({new CategoriesModel(categoriesName)})) {
            mSetCategoriesListWidgetList();
            for (int j = 0; j < Global::categoriesModelList.length(); ++j) {
                if (Global::categoriesModelList[j]->getName() == categoriesName) {
                    on_listWidget_categories_itemClicked(ui->listWidget_categories->item(j));
                    break;
                }
            }

            onActionRenameCategoriesTriggered();
            setCategoriesListEnabled(false);
            break;
        }

        if (!lineEditSearchCategories.isEmpty()) {
            break;
        }
    }
}

void MainWindow::onActionRenameCategoriesTriggered()
{
    QWidget *widget = ui->listWidget_categories->itemWidget(ui->listWidget_categories->selectedItems()[0]);
    QLineEdit *lineEdit_name = widget->findChild<QWidget *>("widget")->findChild<QLineEdit *>("lineEdit_name");
    lineEdit_name->setEnabled(true);
    lineEdit_name->setFocus();
    lineEdit_name->selectAll();

    connect(lineEdit_name, SIGNAL(editingFinished()), this, SLOT(onLineEditNameCategoriesEditingFinished()));
}

void MainWindow::on_pushButton_removeCategories_clicked()
{
    auto selectedIndexes = ui->listWidget_categories->selectionModel()->selectedIndexes();
    if (selectedIndexes.length() != 0) {
        int index = selectedIndexes[0].row();
        auto categoriesModel = ui->listWidget_categories->item(index)->data(Qt::UserRole).value<CategoriesModel *>();
        if (categoriesModel->getCount() == 0) {
            Global::categoriesModelList.removeOne(categoriesModel);
            Global::saveCategoriesModelList();
            mSetCategoriesListWidgetList();
        }
        else {
            QMessageBox::about(this, tr("消息提醒"), tr("该笔记本存在笔记, 请将笔记移出笔记本"));
        }
    }
}

void MainWindow::on_listWidget_categories_customContextMenuRequested(const QPoint &pos)
{
    if (ui->listWidget_categories->itemAt(pos) != NULL) {
        on_listWidget_categories_itemClicked(ui->listWidget_categories->itemAt(pos));
        createListWidgetCategoriesMenu()->exec(ui->listWidget_categories->mapToGlobal(pos));
    }
}

void MainWindow::mSetCategoriesListWidgetList()
{
    ui->listWidget_categories->clear();
    for (auto &&categoriesModel : Global::categoriesModelList) {
        QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->listWidget_categories);
        listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(categoriesModel));
        ui->listWidget_categories->addItem(listWidgetItem);

        CategoriesListCell *categoriesListCell = new CategoriesListCell(categoriesModel);
        listWidgetItem->setSizeHint(categoriesListCell->sizeHint());
        ui->listWidget_categories->setItemWidget(listWidgetItem, categoriesListCell);
    }

    // todo: 为了修复一个listWidget的bug而添加的一段无用代码
    //       因为最后一个item会出现有一半不响应型号, 所以在最后添加了一个无用的item占位
    if (Global::categoriesModelList.length() != 0) {
        ui->listWidget_categories->addItem("bug");
        ui->listWidget_categories->item(Global::categoriesModelList.length())->setHidden(true);
    }
}

void MainWindow::mFiltrateCategoriesListWidgetList()
{
    QString text = ui->lineEdit_searchCategories->displayText();
    for (int i = 0; i < Global::categoriesModelList.length(); ++i) {
        int index = Global::categoriesModelList[i]->getName().indexOf(text, 0, Qt::CaseInsensitive);
        QListWidgetItem *listWidgetItem = ui->listWidget_categories->item(i);
        listWidgetItem->setHidden(text.isEmpty() ? false : index == -1);
    }
}

void MainWindow::on_listWidget_categories_itemDoubleClicked(QListWidgetItem *item)
{
    Global::configModel->setCategoriesName(item->data(Qt::UserRole).value<CategoriesModel *>()->getName());

    mSetTableWidgetList();
    mSetOpenedNoteModel();
    mSelectedSidebarButtonByName(0);
}

void MainWindow::on_lineEdit_searchCategories_textChanged(const QString &arg1)
{
    mFiltrateCategoriesListWidgetList();
}

void MainWindow::on_pushButton_changeTags_clicked()
{
    tagWidget = new TagsWidget(this);
    tagWidget->open();

    connect(tagWidget, SIGNAL(tagsChanged()), this, SLOT(onTagsChange()));
}

void MainWindow::onTagsChange()
{
    mSetOpenedNoteModel();
    mSetEditorModified(true);
    on_action_saveNote_triggered();
}

void MainWindow::on_pushButton_tags_clicked()
{
    mSelectedSidebarButtonByName(2);

    mSetTagsListWidgetList();
}

void MainWindow::on_pushButton_trash_clicked()
{
    mSelectedSidebarButtonByName(3);
}

void MainWindow::on_pushButton_sync_clicked()
{
    mSelectedSidebarButtonByName(4);
}

void MainWindow::mSetTagsListWidgetList()
{
    ui->listWidget_tags->clear();
    for (auto &&tagsModel : Global::tagsModelList) {
        QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->listWidget_tags);
        listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(tagsModel));
        ui->listWidget_tags->addItem(listWidgetItem);

        TagsListCell *tagsListCell = new TagsListCell(tagsModel);
        listWidgetItem->setSizeHint(tagsListCell->sizeHint());
        ui->listWidget_tags->setItemWidget(listWidgetItem, tagsListCell);
    }

    // todo: 为了修复一个listWidget的bug而添加的一段无用代码
    //       因为最后一个item会出现有一半不响应型号, 所以在最后添加了一个无用的item占位
    if (Global::tagsModelList.length() != 0) {
        ui->listWidget_tags->addItem("bug");
        ui->listWidget_tags->item(Global::tagsModelList.length())->setHidden(true);
    }
}

void MainWindow::mFiltrateTagsListWidgetList()
{
    QString text = ui->lineEdit_searchTags->displayText();
    for (int i = 0; i < Global::tagsModelList.length(); ++i) {
        int index = Global::tagsModelList[i]->getName().indexOf(text, 0, Qt::CaseInsensitive);
        QListWidgetItem *listWidgetItem = ui->listWidget_tags->item(i);
        listWidgetItem->setHidden(text.isEmpty() ? false : index == -1);
    }
}

void MainWindow::on_listWidget_tags_itemClicked(QListWidgetItem *item)
{
    QWidget *widget = ui->listWidget_tags->itemWidget(item)->findChild<QWidget *>("widget");

    for (int i = 0; i < Global::tagsModelList.length(); ++i) {
        QListWidgetItem *listWidgetItem = ui->listWidget_tags->item(i);
        QWidget *widget2 = ui->listWidget_tags->itemWidget(listWidgetItem)->findChild<QWidget *>("widget");
        if (widget == widget2) {
            Tools::changeWidgetBorder(widget2, "#7EBFF5", 2);
        }
        else {
            Tools::changeWidgetBorder(widget2, "#DFDFE0", 1);
        }
    }
    ui->listWidget_tags->setItemSelected(item, true);
}

void MainWindow::on_listWidget_tags_itemDoubleClicked(QListWidgetItem *item)
{
    Global::configModel->setTagsName(item->data(Qt::UserRole).value<TagsModel *>()->getName());

    mSetTableWidgetList();
    mSetOpenedNoteModel();
    mSelectedSidebarButtonByName(0);
}

void MainWindow::on_listWidget_tags_customContextMenuRequested(const QPoint &pos)
{
    if (ui->listWidget_tags->itemAt(pos) != NULL) {
        on_listWidget_tags_itemClicked(ui->listWidget_tags->itemAt(pos));
        createListWidgetTagsMenu()->exec(ui->listWidget_tags->mapToGlobal(pos));
    }
}

QMenu *MainWindow::createListWidgetTagsMenu()
{
    QMenu *popMenu = new QMenu(this);

    QAction *action_createTags;
    QAction *action_renameTags;
    QAction *action_removeTags;
    QAction *action_nameSort;
    QAction *action_countSort;
    QAction *action_timeSort;

    action_createTags = new QAction("新建标签", this);
    action_renameTags = new QAction("重命名标签", this);
    action_removeTags = new QAction("删除笔记本", this);
    action_nameSort = new QAction("按标签名称排序", this);
    action_countSort = new QAction("按笔记数量排序", this);
    action_timeSort = new QAction("按笔记更新时间排序", this);

    action_createTags->setObjectName(QStringLiteral("action_createTags"));
    action_renameTags->setObjectName(QStringLiteral("action_renameTags"));
    action_removeTags->setObjectName(QStringLiteral("action_removeTags"));
    action_nameSort->setObjectName(QStringLiteral("action_nameSort"));
    action_countSort->setObjectName(QStringLiteral("action_countSort"));
    action_timeSort->setObjectName(QStringLiteral("action_timeSort"));

    popMenu->addAction(action_createTags);
    popMenu->addAction(action_renameTags);
    popMenu->addAction(action_removeTags);
    popMenu->addSeparator();
    popMenu->addAction(action_nameSort);
    popMenu->addAction(action_countSort);
    popMenu->addAction(action_timeSort);

    connect(action_createTags, &QAction::triggered, this, &MainWindow::on_pushButton_addTags_clicked);
    connect(action_renameTags, &QAction::triggered, this, &MainWindow::onActionRenameTagsTriggered);
    connect(action_removeTags, &QAction::triggered, this, &MainWindow::on_pushButton_removeTags_clicked);
//    connect(action_nameSort, &QAction::triggered, this, &MainWindow::onActionNameSortTriggered);
//    connect(action_countSort, &QAction::triggered, this, &MainWindow::onActionCountSortTriggered);
//    connect(action_timeSort, &QAction::triggered, this, &MainWindow::onActionTimeSortTriggered);

    return popMenu;
}

void MainWindow::on_pushButton_addTags_clicked()
{
    QString tagName;
    QString lineEditSearchTags = ui->lineEdit_searchTags->displayText();

    for (int i = 0; i < 100; ++i) {
        tagName = lineEditSearchTags.isEmpty() ? tr("新建标签%1").arg(i == 0 ? "" : QString::number(i))
                                               : lineEditSearchTags;

        if (Global::addTagsModelList({new TagsModel(tagName)})) {
            mSetTagsListWidgetList();
            for (int j = 0; j < Global::tagsModelList.length(); ++j) {
                if (Global::tagsModelList[j]->getName() == tagName) {
                    on_listWidget_tags_itemClicked(ui->listWidget_tags->item(j));
                    break;
                }
            }

            onActionRenameTagsTriggered();
            setTagsListEnabled(false);
            break;
        }

        if (!lineEditSearchTags.isEmpty()) {
            break;
        }
    }
}

void MainWindow::on_pushButton_removeTags_clicked()
{
    auto selectedIndexes = ui->listWidget_tags->selectionModel()->selectedIndexes();
    if (selectedIndexes.length() != 0) {
        int index = selectedIndexes[0].row();
        auto tagsModel = ui->listWidget_tags->item(index)->data(Qt::UserRole).value<TagsModel *>();
        if (tagsModel->getCount() == 0) {
            Global::tagsModelList.removeOne(tagsModel);
            Global::saveTagsModelList();
            mSetTagsListWidgetList();
        }
        else {
            QMessageBox::about(this, tr("消息提醒"), tr("该笔记本存在笔记, 请将笔记移出笔记本"));
        }
    }
}

void MainWindow::onActionRenameTagsTriggered()
{
    QWidget *widget = ui->listWidget_tags->itemWidget(ui->listWidget_tags->selectedItems()[0]);
    QLineEdit *lineEdit_name = widget->findChild<QWidget *>("widget")->findChild<QLineEdit *>("lineEdit_nameTags");
    lineEdit_name->setEnabled(true);
    lineEdit_name->setFocus();
    lineEdit_name->selectAll();
    connect(lineEdit_name, SIGNAL(editingFinished()), this, SLOT(onLineEditNameTagsEditingFinished()));
}

void MainWindow::onLineEditNameTagsEditingFinished()
{
    int index = ui->listWidget_tags->selectionModel()->selectedIndexes()[0].row();
    QListWidgetItem *listWidgetItem = ui->listWidget_tags->item(index);
    TagsModel *tagsModel = listWidgetItem->data(Qt::UserRole).value<TagsModel *>();
    QWidget *widget = ui->listWidget_tags->itemWidget(ui->listWidget_tags->selectedItems()[0]);
    QLineEdit *lineEdit_name = widget->findChild<QWidget *>("widget")->findChild<QLineEdit *>("lineEdit_nameTags");
    QString name = lineEdit_name->displayText();

    if (!name.isEmpty() && !Global::hasInTagsModelList(name)) {
        if (tagsModel->getName() != name) {
            Global::renameTagsModelListInName(tagsModel->getName(), name);
            tagsModel->setName(name);
        }
    }

    lineEdit_name->setText(tagsModel->getName());
    lineEdit_name->setEnabled(false);
    setTagsListEnabled(true);
}

void MainWindow::on_lineEdit_searchTags_textChanged(const QString &arg1)
{
    mFiltrateTagsListWidgetList();
}

void MainWindow::setCategoriesListEnabled(bool b)
{
    ui->pushButton_addCategories->setEnabled(b);
    ui->lineEdit_searchCategories->setEnabled(b);
    ui->pushButton_removeCategories->setEnabled(b);

}

void MainWindow::setTagsListEnabled(bool b)
{
    ui->pushButton_addTags->setEnabled(b);
    ui->lineEdit_searchTags->setEnabled(b);
    ui->pushButton_removeTags->setEnabled(b);

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    if (ui->stackedWidget->currentIndex() == 1) {
        for (int i = 0; i < Global::categoriesModelList.length(); ++i) {
            QListWidgetItem *listWidgetItem = ui->listWidget_categories->item(i);
            QWidget *widget2 = ui->listWidget_categories->itemWidget(listWidgetItem)->findChild<QWidget *>("widget");
            Tools::changeWidgetBorder(widget2, "#DFDFE0", 1);
            listWidgetItem->setData(Qt::UserRole, QVariant());
            ui->listWidget_categories->setItemSelected(listWidgetItem, false);
        }
    }
    else if (ui->stackedWidget->currentIndex() == 2) {
        for (int i = 0; i < Global::tagsModelList.length(); ++i) {
            QListWidgetItem *listWidgetItem = ui->listWidget_tags->item(i);
            QWidget *widget2 = ui->listWidget_tags->itemWidget(listWidgetItem)->findChild<QWidget *>("widget");
            Tools::changeWidgetBorder(widget2, "#DFDFE0", 1);
            listWidgetItem->setData(Qt::UserRole, QVariant());
            ui->listWidget_tags->setItemSelected(listWidgetItem, false);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *size)
{
    QWidget::resizeEvent(size);

    emit resizeChildWindow(size->size());
}

void MainWindow::on_lineEdit_searchNoteList_textChanged(const QString &arg1)
{
    mFiltrateTableWidgetList();
}

void MainWindow::mSelectedSidebarButtonByName(int i)
{
    QList<QPushButton *> menuPushButtonList = {ui->pushButton_notes, ui->pushButton_folder,
                                               ui->pushButton_tags, ui->pushButton_trash, ui->pushButton_sync};

    for (int j = 0; j < menuPushButtonList.length(); ++j) {
        menuPushButtonList[j]->setChecked(j == i);
    }
    ui->stackedWidget->setCurrentIndex(i);
}

void MainWindow::mInitAppMenu()
{
    QMenu *aboutMenu = new QMenu(ui->menuBar);
    QAction *aboutAction = aboutMenu->addAction(tr("&关于 Gitnoter"));
    aboutAction->setMenuRole(QAction::AboutRole);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(openAboutDialog()));
    menuBar()->addMenu(aboutMenu);

    QMenu *preferencesMenu = new QMenu(ui->menuBar);
    QAction *preferencesAction = preferencesMenu->addAction(tr("&Preferences..."));
    preferencesAction->setMenuRole(QAction::PreferencesRole);
    connect(preferencesAction, SIGNAL(triggered()), this, SLOT(openSettingDialog()));
    menuBar()->addMenu(preferencesMenu);
}

void MainWindow::openAboutDialog()
{
    if (aboutDialog->isHidden()) {
        aboutDialog->exec();
    }
}

void MainWindow::openSettingDialog()
{
    if (settingDialog->isHidden()) {
        settingDialog->open();
    }
}

void MainWindow::onAutoSyncRepoTimerStart()
{
    qDebug() << "onAutoSyncRepoTimerStart";
    int autoSyncRepo = Global::configModel->getAutoSyncRepoTime();
    if (autoSyncRepo) {
        connect(autoSyncRepoTimer, SIGNAL(timeout()), this, SLOT(onAutoSyncRepoTimeout()));
        autoSyncRepoTimer->start(autoSyncRepo);
    }
    else if (autoSyncRepoTimer->isActive()){
        autoSyncRepoTimer->stop();
    }
}

void MainWindow::onAutoLockTimerStart()
{
    int autoLockTime = Global::configModel->getAutoLockTime();
    if (autoLockTime) {
        connect(autoLockTimer, SIGNAL(timeout()), this, SLOT(onAutoLockTimeout()));
        autoLockTimer->start(autoLockTime);
    }
    else if (autoLockTimer->isActive()){
        autoLockTimer->stop();
    }
}

void MainWindow::onAutoSyncRepoTimeout()
{
    if (Global::configModel->getLocalRepoStatus() == 1) {
        Global::gitManager->commitA();
        Global::gitManager->pull();
        Global::gitManager->commitA();
        Global::gitManager->push();
    }
}

void MainWindow::onAutoLockTimeout()
{
    if (autoLockTimer->isActive()){
        autoLockTimer->stop();
    }

    close();
    (new LockDialog())->show();
}

void MainWindow::onEditorFontChange()
{
    QFont font(Global::configModel->getFontFamily());
    font.setPixelSize(Global::configModel->getFontPixelSize());
    ui->plainTextEdit_editor->setFont(font);
}

