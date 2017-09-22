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
    gInitConfigModel();
    mInitTableWidgetList();
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
    updatePreview();
}

void MainWindow::updatePreview()
{
    if (gOpenNoteModel->contentModel->getTitle().isEmpty()) {
        m_content.setText(gOpenNoteModel->contentModel->getBody());
    }
    else {
        m_content.setText("# " + gOpenNoteModel->contentModel->getTitle() + "\n\n"
                          + gOpenNoteModel->contentModel->getBody());
    }
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

    setEditText();
    setModified(false);

    setTagsData();
    ui->pushButton_categories->setText(gOpenNoteModel->categoriesModel->getName());
}

void MainWindow::mInitTableWidgetList(QString text)
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
}

void MainWindow::filtrateTableWidgetList(QString text)
{
    int firstShowRowIndex = 0;
    bool hasOpenNoteInList = false;

    for (int row = 0; row < ui->tableWidget_list->rowCount(); ++row) {
        for (int column = 0; column < ui->tableWidget_list->columnCount(); ++column) {
            QTableWidgetItem *tableWidgetItem = ui->tableWidget_list->item(row, column);
            NoteModel *noteModel = tableWidgetItem->data(Qt::UserRole).value<NoteModel *>();
            bool hidden = true;

            if (gConfigModel->getIsSelectedClasses() == 1) {
                if (noteModel->categoriesModel->getName() == gConfigModel->getCategoriesName()) {
                    hidden = false;
                }
            }
            else if (gConfigModel->getIsSelectedClasses() == 2) {
                for (auto &&tagsModel : *noteModel->tagsModelList) {
                    if (tagsModel->getName() == gConfigModel->getTagsName()) {
                        hidden = false;
                        break;
                    }
                }
            }

            if (!text.isEmpty()) {
                if (noteModel->contentModel->getTitle().indexOf(text, 0, Qt::CaseInsensitive) != -1) {
                    hidden = false;
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
    }

    if (!hasOpenNoteInList && firstShowRowIndex > 0) {
        QTableWidgetItem *tableWidgetItem = ui->tableWidget_list->item(firstShowRowIndex, 0);
        NoteModel *noteModel = tableWidgetItem->data(Qt::UserRole).value<NoteModel *>();
        gConfigModel->setOpenNotesUuid(noteModel->contentModel->getUuid());
        ui->tableWidget_list->selectRow(firstShowRowIndex);
    }
    else {
        gOpenNoteModel->clear();
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

void MainWindow::setEditText()
{
    disconnect(ui->plainTextEdit_editor, &QPlainTextEdit::textChanged, this, &MainWindow::textChanged);
    disconnect(ui->lineEdit_title, &QLineEdit::textChanged, this, &MainWindow::textChanged);

    ui->lineEdit_title->setText(gOpenNoteModel->contentModel->getTitle());
    ui->plainTextEdit_editor->setPlainText(gOpenNoteModel->contentModel->getBody());

    setModified(true);
    updatePreview();

    // 监听编辑器中文本是否有更改
    connect(ui->plainTextEdit_editor, &QPlainTextEdit::textChanged, this, &MainWindow::textChanged);
    connect(ui->lineEdit_title, &QLineEdit::textChanged, this, &MainWindow::textChanged);
}

void MainWindow::on_action_saveNote_triggered()
{
    if (!isModified()) {
        return;
    }

    Tools::writerFile(gOpenNoteModel->contentModel->getFilePath(), gOpenNoteModel->getNote());
//    gDatabase->addNoteText(gOpenNoteModel);
    mInitTableWidgetList();
}

void MainWindow::on_action_newNote_triggered()
{
    on_action_saveNote_triggered();

    gOpenNoteModel->clear();
}

bool MainWindow::isModified()
{
    return ui->lineEdit_title->isModified() || ui->plainTextEdit_editor->document()->isModified();
}

void MainWindow::setModified(bool m)
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
//        gDatabase->deleteNoteByUuid(gOpenNoteModel->contentModel->getUuid());
        QFile::remove(gOpenNoteModel->contentModel->getFilePath());
    }
    gConfigModel->setOpenNotesUuid("");
    gOpenNoteModel->clear();

    mInitTableWidgetList();
    mSetOpenedNoteModel();
}

void MainWindow::onHeaderViewSortIndicatorChanged(int logicalIndex, Qt::SortOrder order)
{
    gConfigModel->setSidebarSortKey(logicalIndex);
    gConfigModel->setSidebarSortValue(order == Qt::DescendingOrder ? "DESC" : "ASC");

    qDebug() << "on_headerView_sortIndicatorChanged";
}

void MainWindow::setTagsData()
{
    QString tagsString;
    for (auto &&item : *gOpenNoteModel->tagsModelList) {
        tagsString += item->getName() + gTagSplit;
    }
    tagsString.chop(gTagSplit.length());
    ui->pushButton_changeTags->setText(tagsString);
}

void MainWindow::onChangeCategories()
{
    ui->pushButton_categories->setText(gOpenNoteModel->categoriesModel->getName());
    setModified(true);
    on_action_saveNote_triggered();
}

void MainWindow::on_pushButton_folder_clicked()
{
    mSelectedSidebarButtonByName(sender()->objectName());
    ui->stackedWidget->setCurrentIndex(1);

    if (ui->listWidget_categories->count() > 0) {
        return;
    }

    setCategoriesList();
}

void MainWindow::on_listWidget_categories_itemClicked(QListWidgetItem *item)
{
    QWidget *widget = ui->listWidget_categories->itemWidget(item)->findChild<QWidget *>("widget");
    if (item->data(Qt::UserRole).isNull()) {
        for (int i = 0; i < mCategoriesModelList.length(); ++i) {
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
    mSelectedSidebarButtonByName(sender()->objectName());
    ui->stackedWidget->setCurrentIndex(0);
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
        lineEdit_name->setText(mCategoriesModelList[index]->getName());
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

void MainWindow::setCategoriesList(bool reread, const QString &string)
{
    QList<CategoriesModel *> categoriesModelSearchList;

//    if (mCategoriesModelList.length() == 0 || reread) {
//        mCategoriesModelList = gDatabase->selectCategories();
//    }

    if (!string.isEmpty()) {
        for (int i = 0; i < mCategoriesModelList.length(); ++i) {
            int searchIndex = mCategoriesModelList[i]->getName().indexOf(string, 0, Qt::CaseInsensitive);
            if (searchIndex != -1) {
                categoriesModelSearchList.append(mCategoriesModelList[i]);
            }
        }
        mCategoriesModelList = categoriesModelSearchList;
    }

    ui->listWidget_categories->clear();
    for (auto &&datum : mCategoriesModelList) {
//        datum->setCount(gDatabase->selectNJCByCategoriesId(datum->getCategoriesId()).length());
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget_categories);
        ui->listWidget_categories->addItem(item);
        CategoriesListCell *categoriesListCell = new CategoriesListCell(datum);
        item->setSizeHint(categoriesListCell->sizeHint());
        ui->listWidget_categories->setItemWidget(item, categoriesListCell);
    }

    // todo: 为了修复一个listWidget的bug而添加的一段无用代码
    //       因为最后一个item会出现有一半不响应型号, 所以在最后添加了一个无用的item占位
    if (mCategoriesModelList.length() != 0) {
        ui->listWidget_categories->addItem("bug");
        ui->listWidget_categories->item(mCategoriesModelList.length())->setHidden(true);
    }
}

void MainWindow::on_listWidget_categories_doubleClicked(const QModelIndex &index)
{
    gConfigModel->setCategoriesName(mCategoriesModelList[index.row()]->getName());

    mInitTableWidgetList();
    mSetOpenedNoteModel();

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_lineEdit_searchCategories_textChanged(const QString &arg1)
{
    arg1.isEmpty() ? setCategoriesList() : setCategoriesList(false, arg1);
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
    setTagsData();
    setModified(true);
    on_action_saveNote_triggered();
}

void MainWindow::on_pushButton_tags_clicked()
{
    mSelectedSidebarButtonByName(sender()->objectName());
    ui->stackedWidget->setCurrentIndex(2);

    if (ui->listWidget_tags->count() > 0) {
        return;
    }

    setTagsList();
}

void MainWindow::on_pushButton_sync_clicked()
{
    mSelectedSidebarButtonByName(sender()->objectName());
}

void MainWindow::on_pushButton_trash_clicked()
{
    mSelectedSidebarButtonByName(sender()->objectName());
}

void MainWindow::setTagsList(bool reread, const QString &string)
{
    QList<TagsModel *> tagsModelSearchList;

    if (mtagsModelList.length() == 0 || reread) {
//        mtagsModelList = gDatabase->selectTags();
    }

    if (!string.isEmpty()) {
        for (int i = 0; i < mtagsModelList.length(); ++i) {
            int searchIndex = mtagsModelList[i]->getName().indexOf(string, 0, Qt::CaseInsensitive);
            if (searchIndex != -1) {
                tagsModelSearchList.append(mtagsModelList[i]);
            }
        }
        mtagsModelList = tagsModelSearchList;
    }

    ui->listWidget_tags->clear();
    for (auto &&datum : mtagsModelList) {
//        datum->setCount(gDatabase->selectNJTByTagsId(datum->getTagsId()).length());
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget_tags);
        ui->listWidget_tags->addItem(item);
        TagsListCell *tagsListCell = new TagsListCell(datum);
        item->setSizeHint(tagsListCell->sizeHint());
        ui->listWidget_tags->setItemWidget(item, tagsListCell);
    }

    // todo: 为了修复一个listWidget的bug而添加的一段无用代码
    //       因为最后一个item会出现有一半不响应型号, 所以在最后添加了一个无用的item占位
    if (mtagsModelList.length() != 0) {
        ui->listWidget_tags->addItem("bug");
        ui->listWidget_tags->item(mtagsModelList.length())->setHidden(true);
    }
}

void MainWindow::on_listWidget_tags_itemClicked(QListWidgetItem *item)
{
    QWidget *widget = ui->listWidget_tags->itemWidget(item)->findChild<QWidget *>("widget");
    if (item->data(Qt::UserRole).isNull()) {
        for (int i = 0; i < mtagsModelList.length(); ++i) {
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

void MainWindow::on_listWidget_tags_doubleClicked(const QModelIndex &index)
{
    gConfigModel->setTagsName(mtagsModelList[index.row()]->getName());
    mInitTableWidgetList();
    mSetOpenedNoteModel();

    ui->stackedWidget->setCurrentIndex(0);
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
        lineEdit_nameTags->setText(mtagsModelList[index]->getName());
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
    arg1.isEmpty() ? setTagsList() : setTagsList(false, arg1);
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
        for (int i = 0; i < mCategoriesModelList.length(); ++i) {
            QListWidgetItem *listWidgetItem = ui->listWidget_categories->item(i);
            QWidget *widget2 = ui->listWidget_categories->itemWidget(listWidgetItem)->findChild<QWidget *>("widget");
            Tools::changeWidgetBorder(widget2, "#DFDFE0", 1);
            listWidgetItem->setData(Qt::UserRole, QVariant());
            ui->listWidget_categories->setItemSelected(listWidgetItem, false);
        }
    }
    else if (ui->stackedWidget->currentIndex() == 2) {
        for (int i = 0; i < mtagsModelList.length(); ++i) {
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
    mInitTableWidgetList(arg1);
}


void MainWindow::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);

    qDebug() << "1";
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QWidget::dropEvent(event);

    qDebug() << "2";
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    QWidget::dragMoveEvent(event);

    qDebug() << "3";
}

void MainWindow::mSelectedSidebarButtonByName(const QString &name)
{
    QMap<QString, QPushButton *>menuPushButtonList = {
            {"pushButton_folder", ui->pushButton_folder},
            {"pushButton_notes", ui->pushButton_notes},
            {"pushButton_tags", ui->pushButton_tags},
            {"pushButton_trash", ui->pushButton_trash},
            {"pushButton_sync", ui->pushButton_sync}
    };

    for (auto iterator = menuPushButtonList.begin(); iterator != menuPushButtonList.end(); ++iterator) {
        iterator.value()->setChecked(iterator.key() == name);
    }
}

