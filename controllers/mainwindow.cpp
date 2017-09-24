#include "mainwindow.h"
#include "categorieslistcell.h"
#include "tagslistcell.h"

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

    ui->listWidget_categories->setAcceptDrops(false);   // 不接受拖放 ui编辑器编辑无用代码, 可能是bug
    ui->lineEdit_searchNote->setHidden(true);           // 隐藏暂时不用的组件

    // 屏蔽选中时的边框颜色
    ui->lineEdit_title->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->listWidget_categories->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->listWidget_tags->setAttribute(Qt::WA_MacShowFocusRect, 0);

    connect(ui->tableWidget_list->horizontalHeader(), &QHeaderView::sortIndicatorChanged,
            this, &MainWindow::onHeaderViewSortIndicatorChanged);

    gInitNoteModelList();
    gInitCategoriesModelList();
    gInitTagsModelList();
    gInitConfigModel();
    mSetTableWidgetList();
    mSetOpenedNoteModel(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::textChanged()
{
    gOpenNoteModel->contentModel->setTitle(ui->lineEdit_title->displayText().isEmpty()
                                           ? ui->lineEdit_title->placeholderText()
                                           : ui->lineEdit_title->displayText());
    gOpenNoteModel->contentModel->setBody(ui->plainTextEdit_editor->toPlainText());
    gOpenNoteModel->contentModel->setUpdateDate(0);
    m_content.setText(gOpenNoteModel->getDisplayNote());
}

void MainWindow::on_pushButton_categories_clicked()
{
    categoriesWidget = new CategoriesWidget(this);
    categoriesWidget->show();

    connect(this, &MainWindow::resizeChildWindow, categoriesWidget, &CategoriesWidget::resizeWindow);
    connect(categoriesWidget, SIGNAL(changeCategories()), this, SLOT(onChangeCategories()));
}

void MainWindow::mSetOpenedNoteModel(bool initEditor)
{
    if (gConfigModel->getOpenNotesUuid().isEmpty()) {
        if (gNoteModelList.length() != 0) {
            gConfigModel->setOpenNotesUuid(gNoteModelList.at(0)->contentModel->getUuid());
            mSetOpenedNoteModel();
            return;
        }
        gOpenNoteModel = new NoteModel(Tools::readerFile(":/marked/default.md"));
    }
    else {
        gOpenNoteModel = gGetNoteModelByUuid(gConfigModel->getOpenNotesUuid());
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

    ui->lineEdit_title->setText(gOpenNoteModel->contentModel->getTitle());
    ui->plainTextEdit_editor->setPlainText(gOpenNoteModel->contentModel->getBody());
    m_content.setText(gOpenNoteModel->getDisplayNote());

    // 监听编辑器中文本是否有更改
    connect(ui->plainTextEdit_editor, &QPlainTextEdit::textChanged, this, &MainWindow::textChanged);
    connect(ui->lineEdit_title, &QLineEdit::textChanged, this, &MainWindow::textChanged);
    mSetEditorModified(false);

    ui->pushButton_changeTags->setText(gOpenNoteModel->getTagsString());
    ui->pushButton_categories->setText(gOpenNoteModel->categoriesModel->getName());
}

void MainWindow::mSetTableWidgetList()
{
    ui->tableWidget_list->clearContents();

    ui->tableWidget_list->setRowCount(gNoteModelList.length());
    for (int i = 0; i < gNoteModelList.length(); ++i) {
        auto *tableWidgetItem0 = new QTableWidgetItem(gNoteModelList[i]->contentModel->getTitle());
        tableWidgetItem0->setData(Qt::UserRole, QVariant::fromValue(gNoteModelList[i]));
        auto *tableWidgetItem1 = new QTableWidgetItem(
                Tools::timestampToDateTime(gNoteModelList[i]->contentModel->getUpdateDate()));
        tableWidgetItem1->setData(Qt::UserRole, QVariant::fromValue(gNoteModelList[i]));
        ui->tableWidget_list->setItem(i, 0, tableWidgetItem0);
        ui->tableWidget_list->setItem(i, 1, tableWidgetItem1);
    }

    int sidebarSortKey = gConfigModel->getSidebarSortKey();
    Qt::SortOrder sidebarSortValue = gConfigModel->getSidebarSortValue() == "DESC" ? Qt::DescendingOrder
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

        qDebug() << noteModel->categoriesModel->getName() << "==" << gConfigModel->getCategoriesName();
        if (gConfigModel->getIsSelectedClasses() == 1) {
            if (noteModel->categoriesModel->getName() == gConfigModel->getCategoriesName()) {
                hidden = false;
                if (!text.isEmpty()) {
                    hidden = noteModel->contentModel->getTitle().indexOf(text, 0, Qt::CaseInsensitive) == -1;
                }
            }
        }
        else if (gConfigModel->getIsSelectedClasses() == 2) {
            for (auto &&tagsModel : *noteModel->tagsModelList) {
                if (tagsModel->getName() == gConfigModel->getTagsName()) {
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
            if (!hasOpenNoteInList && noteModel->contentModel->getUuid() == gConfigModel->getOpenNotesUuid()) {
                ui->tableWidget_list->selectRow(row);
                hasOpenNoteInList = true;
            }
        }
    }

    if (!hasOpenNoteInList && firstShowRowIndex > 0) {
        QTableWidgetItem *tableWidgetItem = ui->tableWidget_list->item(firstShowRowIndex, 0);
        NoteModel *noteModel = tableWidgetItem->data(Qt::UserRole).value<NoteModel *>();
        gConfigModel->setOpenNotesUuid(noteModel->contentModel->getUuid());
        ui->tableWidget_list->selectRow(firstShowRowIndex);
    }
    else {
        gConfigModel->setOpenNotesUuid("");
    }
}

void MainWindow::on_tableWidget_list_itemClicked(QTableWidgetItem *item)
{
    QString uuid = item->data(Qt::UserRole).value<NoteModel *>()->contentModel->getUuid();
    if (gConfigModel->getOpenNotesUuid() == uuid) {
        return;
    }
    gConfigModel->setOpenNotesUuid(uuid);
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

    Tools::writerFile(gOpenNoteModel->contentModel->getFilePath(), gOpenNoteModel->getNote());
    mSetTableWidgetList();
}

void MainWindow::on_action_newNote_triggered()
{
    on_action_saveNote_triggered();
    gOpenNoteModel->clear();
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
    if (!gOpenNoteModel->contentModel->getUuid().isEmpty()) {
        QFile::remove(gOpenNoteModel->contentModel->getFilePath());
    }
    gConfigModel->setOpenNotesUuid("");

    mSetTableWidgetList();
    mSetOpenedNoteModel();
}

void MainWindow::onHeaderViewSortIndicatorChanged(int logicalIndex, Qt::SortOrder order)
{
    gConfigModel->setSidebarSortKey(logicalIndex);
    gConfigModel->setSidebarSortValue(order == Qt::DescendingOrder ? "DESC" : "ASC");

    qDebug() << "on_headerView_sortIndicatorChanged";
}

void MainWindow::onChangeCategories()
{
    mSetOpenedNoteModel();
    mSetEditorModified(true);
    on_action_saveNote_triggered();
}

void MainWindow::on_pushButton_folder_clicked()
{
    mSelectedSidebarButtonByName(1);

    if (ui->listWidget_categories->count() > 0) {
        return;
    }

    mSetCategoriesListWidgetList();
}

void MainWindow::on_listWidget_categories_itemClicked(QListWidgetItem *item)
{
    QWidget *widget = ui->listWidget_categories->itemWidget(item)->findChild<QWidget *>("widget");
    if (item->data(Qt::UserRole).isNull()) {
        for (int i = 0; i < gCategoriesModelList.length(); ++i) {
            QListWidgetItem *listWidgetItem = ui->listWidget_categories->item(i);
            QWidget *widget2 = ui->listWidget_categories->itemWidget(listWidgetItem)->findChild<QWidget *>("widget");
            Tools::changeWidgetBorder(widget2, "#DFDFE0", 1);
            listWidgetItem->setData(Qt::UserRole, QVariant());
            ui->listWidget_categories->setItemSelected(listWidgetItem, false);
        }

        item->setData(Qt::UserRole, 1);
        Tools::changeWidgetBorder(widget, "#7EBFF5", 2);
        ui->listWidget_categories->setItemSelected(item, true);
    }
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

void MainWindow::onLineEditNameEditingFinished()
{
    int index = ui->listWidget_categories->selectionModel()->selectedIndexes()[0].row();
    QWidget *widget = ui->listWidget_categories->itemWidget(ui->listWidget_categories->selectedItems()[0]);
    QLineEdit *lineEdit_name = widget->findChild<QWidget *>("widget")->findChild<QLineEdit *>("lineEdit_name");

    if (lineEdit_name->displayText().isEmpty()) {
        lineEdit_name->setText(gCategoriesModelList[index]->getName());
    }
    else {
//        if (gDatabase->updateCategoriesByName(lineEdit_name->displayText(),
//                                                    mCategoriesModelList[index]->getName())) {
//            auto categoriesList = gDatabase->selectNJCByCategoriesId(
//                    mCategoriesModelList[index]->getCategoriesId());
//            for (auto &&item : categoriesList) {
//                auto *note = gDatabase->getNoteByUuid(item->getNotesUuid());
//                Tools::writerFile(note->contentModel->getFilePath(), note->getNote());
//            }
//
//            mCategoriesModelList[index]->setName(lineEdit_name->displayText());
//        }
//        else {
//            lineEdit_name->setText(mCategoriesModelList[index]->getName());
//        }
    }
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

//        if (gDatabase->insertCategories(categoriesName) != 0) {
//            setCategoriesList(true, lineEditSearchCategories);
//
//            for (int j = 0; j < mCategoriesModelList.length(); ++j) {
//                if (mCategoriesModelList[j]->getName() == categoriesName) {
//                    on_listWidget_categories_itemClicked(ui->listWidget_categories->item(j));
//                    break;
//                }
//            }
//
//            onActionRenameCategoriesTriggered();
//            setCategoriesListEnabled(false);
//            break;
//        }

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
    connect(lineEdit_name, SIGNAL(editingFinished()), this, SLOT(onLineEditNameEditingFinished()));
}

void MainWindow::on_pushButton_removeCategories_clicked()
{
    auto selectedIndexes = ui->listWidget_categories->selectionModel()->selectedIndexes();
    if (selectedIndexes.length() != 0) {
        int index = selectedIndexes[0].row();

//        if (mCategoriesModelList[index]->getCount() == 0) {
//            gDatabase->deleteCategoriesByName(mCategoriesModelList[index]->getName());
//            setCategoriesList(true, ui->lineEdit_searchCategories->displayText());
//        }
//        else {
//            QMessageBox::about(this, tr("消息提醒"), tr("该笔记本存在笔记, 请将笔记移出笔记本"));
//        }
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
    for (auto &&categoriesModel : gCategoriesModelList) {
        QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->listWidget_categories);
        listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(categoriesModel));
        ui->listWidget_categories->addItem(listWidgetItem);

        CategoriesListCell *categoriesListCell = new CategoriesListCell(categoriesModel);
        listWidgetItem->setSizeHint(categoriesListCell->sizeHint());
        ui->listWidget_categories->setItemWidget(listWidgetItem, categoriesListCell);
    }

    // todo: 为了修复一个listWidget的bug而添加的一段无用代码
    //       因为最后一个item会出现有一半不响应型号, 所以在最后添加了一个无用的item占位
    if (gCategoriesModelList.length() != 0) {
        ui->listWidget_categories->addItem("bug");
        ui->listWidget_categories->item(gCategoriesModelList.length())->setHidden(true);
    }
}

void MainWindow::mFiltrateCategoriesListWidgetList()
{
    QString text = ui->lineEdit_searchCategories->displayText();
    for (int i = 0; i < gCategoriesModelList.length(); ++i) {
        int index = gCategoriesModelList[i]->getName().indexOf(text, 0, Qt::CaseInsensitive);
        QListWidgetItem *listWidgetItem = ui->listWidget_categories->item(i);
        listWidgetItem->setHidden(text.isEmpty() ? false : index == -1);
    }
}

void MainWindow::on_listWidget_categories_itemDoubleClicked(QListWidgetItem *item)
{
    gConfigModel->setCategoriesName(item->data(Qt::UserRole).value<CategoriesModel *>()->getName());

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
    tagWidget->show();

    connect(this, &MainWindow::resizeChildWindow, tagWidget, &TagsWidget::resizeWindow);
    connect(tagWidget, SIGNAL(changeTags()), this, SLOT(onChangeTags()));
}

void MainWindow::onChangeTags()
{
    mSetOpenedNoteModel();
    mSetEditorModified(true);
    on_action_saveNote_triggered();
}

void MainWindow::on_pushButton_tags_clicked()
{
    mSelectedSidebarButtonByName(2);

    if (ui->listWidget_tags->count() > 0) {
        return;
    }

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
    for (auto &&tagsModel : gTagsModelList) {
        QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->listWidget_tags);
        listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(tagsModel));
        ui->listWidget_tags->addItem(listWidgetItem);

        TagsListCell *tagsListCell = new TagsListCell(tagsModel);
        listWidgetItem->setSizeHint(tagsListCell->sizeHint());
        ui->listWidget_tags->setItemWidget(listWidgetItem, tagsListCell);
    }

    // todo: 为了修复一个listWidget的bug而添加的一段无用代码
    //       因为最后一个item会出现有一半不响应型号, 所以在最后添加了一个无用的item占位
    if (gTagsModelList.length() != 0) {
        ui->listWidget_tags->addItem("bug");
        ui->listWidget_tags->item(gTagsModelList.length())->setHidden(true);
    }
}

void MainWindow::mFiltrateTagsListWidgetList()
{
    QString text = ui->lineEdit_searchTags->displayText();
    for (int i = 0; i < gTagsModelList.length(); ++i) {
        int index = gTagsModelList[i]->getName().indexOf(text, 0, Qt::CaseInsensitive);
        QListWidgetItem *listWidgetItem = ui->listWidget_tags->item(i);
        listWidgetItem->setHidden(text.isEmpty() ? false : index == -1);
    }
}

void MainWindow::on_listWidget_tags_itemClicked(QListWidgetItem *item)
{
    QWidget *widget = ui->listWidget_tags->itemWidget(item)->findChild<QWidget *>("widget");
    if (item->data(Qt::UserRole).isNull()) {
        for (int i = 0; i < gTagsModelList.length(); ++i) {
            QListWidgetItem *listWidgetItem = ui->listWidget_tags->item(i);
            QWidget *widget2 = ui->listWidget_tags->itemWidget(listWidgetItem)->findChild<QWidget *>("widget");
            Tools::changeWidgetBorder(widget2, "#DFDFE0", 1);
            listWidgetItem->setData(Qt::UserRole, QVariant());
            ui->listWidget_tags->setItemSelected(listWidgetItem, false);
        }

        item->setData(Qt::UserRole, 1);
        Tools::changeWidgetBorder(widget, "#7EBFF5", 2);
        ui->listWidget_tags->setItemSelected(item, true);
    }
}

void MainWindow::on_listWidget_tags_itemDoubleClicked(QListWidgetItem *item)
{
    gConfigModel->setTagsName(item->data(Qt::UserRole).value<TagsModel *>()->getName());

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

//        if (gDatabase->insertTags(tagName) != 0) {
//            setTagsList(true, lineEditSearchTags);
//
//            for (int j = 0; j < mtagsModelList.length(); ++j) {
//                if (mtagsModelList[j]->getName() == tagName) {
//                    on_listWidget_tags_itemClicked(ui->listWidget_tags->item(j));
//                    break;
//                }
//            }
//
//            onActionRenameTagsTriggered();
//            setTagsListEnabled(false);
//            break;
//        }

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

//        if (mtagsModelList[index]->getCount() == 0) {
//            gDatabase->deleteTagsByName(mtagsModelList[index]->getName());
//            setTagsList(true, ui->lineEdit_searchTags->displayText());
//        }
//        else {
//            QMessageBox::about(this, tr("消息提醒"), tr("该标签内存在笔记, 请先移出"));
//        }
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
    QWidget *widget = ui->listWidget_tags->itemWidget(ui->listWidget_tags->selectedItems()[0]);
    QLineEdit *lineEdit_nameTags = widget->findChild<QWidget *>("widget")->findChild<QLineEdit *>("lineEdit_nameTags");

    if (lineEdit_nameTags->displayText().isEmpty()) {
        lineEdit_nameTags->setText(gTagsModelList[index]->getName());
    }
    else {
//        if (gDatabase->updateTagsByName(lineEdit_nameTags->displayText(),
//                                              mtagsModelList[index]->getName())) {
//            auto tagsList = gDatabase->selectNJTByTagsId(mtagsModelList[index]->getTagsId());
//            for (auto &&item : tagsList) {
//                auto *note = gDatabase->getNoteByUuid(item->getNotesUuid());
//                Tools::writerFile(note->contentModel->getFilePath(), note->getNote());
//            }
//
//            mtagsModelList[index]->setName(lineEdit_nameTags->displayText());
//        }
//        else {
//            lineEdit_nameTags->setText(mtagsModelList[index]->getName());
//        }
    }
    lineEdit_nameTags->setEnabled(false);

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
        for (int i = 0; i < gCategoriesModelList.length(); ++i) {
            QListWidgetItem *listWidgetItem = ui->listWidget_categories->item(i);
            QWidget *widget2 = ui->listWidget_categories->itemWidget(listWidgetItem)->findChild<QWidget *>("widget");
            Tools::changeWidgetBorder(widget2, "#DFDFE0", 1);
            listWidgetItem->setData(Qt::UserRole, QVariant());
            ui->listWidget_categories->setItemSelected(listWidgetItem, false);
        }
    }
    else if (ui->stackedWidget->currentIndex() == 2) {
        for (int i = 0; i < gTagsModelList.length(); ++i) {
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

