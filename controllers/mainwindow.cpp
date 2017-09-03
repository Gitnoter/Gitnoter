#include "mainwindow.h"
#include "previewpage.h"
#include "tools.h"
#include "globals.h"
#include "categorieslistcell.h"

#include "ui_mainwindow.h"

#include <QWebChannel>
#include <QMessageBox>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 不接受拖放 ui编辑器编辑无用必须代码, 可能是bug
    ui->listWidget_categories->setAcceptDrops(false);

//    QFile defaultTextFile(":/marked/default.md");
//    defaultTextFile.open(QIODevice::ReadOnly);
//    QString defaultText = QString(defaultTextFile.readAll());
//
//    noteModel = new NoteModel(defaultText);
//
//
//    return;

//    qDebug() << Qt::DescendingOrder;

    this->initNotesToDatabases();

    g_configTableModel->setOpenNotesUuid("c6c71bef-3dbf-4fd4-ab3c-2a111f58fcde5");
    g_configTableModel->setSidebarSortKey(1);
    g_configTableModel->setSidebarSortValue("DESC");
    g_configTableModel->setCategoriesId(1);

    this->setDefaultNote();
    this->setSidebarTable();

    // 初始化菜单快捷键
    ui->action_newFile->setShortcut(Qt::CTRL | Qt::Key_N);
    ui->action_newCategories->setShortcut(Qt::META | Qt::CTRL | Qt::Key_C);
    ui->action_newTags->setShortcut(Qt::META | Qt::CTRL | Qt::Key_T);

    ui->action_save->setShortcut(Qt::CTRL | Qt::Key_S);
    ui->action_quit->setShortcut(Qt::CTRL | Qt::Key_W);

    ui->action_synch->setShortcut(Qt::META | Qt::CTRL | Qt::Key_S);

    ui->action_repeal->setShortcut(Qt::CTRL | Qt::Key_Z);
    ui->action_reform->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_Z);

    ui->action_cut->setShortcut(Qt::CTRL | Qt::Key_X);
    ui->action_copy->setShortcut(Qt::CTRL | Qt::Key_C);
    ui->action_paste->setShortcut(Qt::CTRL | Qt::Key_V);
    ui->action_selectAll->setShortcut(Qt::CTRL | Qt::Key_A);

    ui->action_notes->setShortcut(Qt::ALT | Qt::CTRL | Qt::Key_F);
    ui->action_noteText->setShortcut(Qt::CTRL | Qt::Key_F);

//    ui->action_toUppercase->setShortcut(Qt::CTRL | Qt::Key_N);
//    ui->action_toLowercase->setShortcut(Qt::CTRL | Qt::Key_N);
//    ui->action_toUppercaseAtFirst->setShortcut(Qt::CTRL | Qt::Key_N);

    connect(ui->action_newFile, &QAction::triggered, this, &MainWindow::onNewFile);
    connect(ui->action_save, &QAction::triggered, this, &MainWindow::onSaveFile);

    connect(ui->tableWidget_list->horizontalHeader(), &QHeaderView::sortIndicatorChanged,
            this, &MainWindow::on_headerView_sortIndicatorChanged);

    // 左侧菜单栏
    this->menuPushButtons.insert("pushButton_folder", ui->pushButton_folder);
    this->menuPushButtons.insert("pushButton_notes", ui->pushButton_notes);
    this->menuPushButtons.insert("pushButton_search", ui->pushButton_search);
    this->menuPushButtons.insert("pushButton_sync", ui->pushButton_sync);
    this->menuPushButtons.insert("pushButton_tags", ui->pushButton_tags);
    this->menuPushButtons.insert("pushButton_Trash", ui->pushButton_Trash);

    for (auto itr = menuPushButtons.begin(); itr != menuPushButtons.end(); ++itr) {
        connect(itr.value(), &QPushButton::clicked, this, &MainWindow::menuPushButtonClicked);
    }

    // 屏蔽选中时的边框颜色
    ui->lineEdit_title->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->listWidget_categories->setAttribute(Qt::WA_MacShowFocusRect, 0);

    // 初始化编辑器
    ui->webEngineView_preview->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->webEngineView_preview->setContextMenuPolicy(Qt::NoContextMenu);

    PreviewPage *page = new PreviewPage(this);
    ui->webEngineView_preview->setPage(page);

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_content);
    page->setWebChannel(channel);

    ui->webEngineView_preview->setUrl(QUrl("qrc:/marked/index.html"));

    this->setMainWindowData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::textChanged()
{
    g_noteModel->noteTableModel->setTitle(ui->lineEdit_title->displayText().isEmpty()
                                        ? ui->lineEdit_title->placeholderText()
                                        : ui->lineEdit_title->displayText());
    g_noteModel->noteTableModel->setBody(ui->plainTextEdit_editor->toPlainText());
    g_noteModel->noteTableModel->setUpdateDate(0);
    this->updatePreview();
}

void MainWindow::updatePreview()
{
    if (g_noteModel->noteTableModel->getTitle().isEmpty()) {
        m_content.setText(g_noteModel->noteTableModel->getBody());
    }
    else {
        m_content.setText("# " + g_noteModel->noteTableModel->getTitle() + "\n\n" + g_noteModel->noteTableModel->getBody());
    }
}

void MainWindow::menuPushButtonClicked()
{
    QString objectName = this->sender()->objectName();

    // TODO: 选择这里需要优化, 重复点击同一个按钮会出现闪烁, 之后改成, 点击替换按钮图片, 避免闪烁
    QMap<QString, QPushButton *>::Iterator itr;
    for (itr = this->menuPushButtons.begin(); itr != this->menuPushButtons.end(); ++itr) {
        itr.value()->setChecked(itr.key() == objectName);
    }
}

void MainWindow::on_pushButton_categories_clicked()
{
    categoriesWidget = new CategoriesWidget(this);
    categoriesWidget->show();

    connect(this, &MainWindow::resizeChildWindow, categoriesWidget, &CategoriesWidget::resizeWindow);
    connect(categoriesWidget, SIGNAL(changeCategories()), this, SLOT(onChangeCategories()));
}

void MainWindow::resizeEvent(QResizeEvent *size) {
    emit resizeChildWindow(size->size());
}

void MainWindow::initNotesToDatabases()
{
    QString notesPath = QDir(g_repoPath).filePath(g_noteFolderName);
    QFileInfoList fileInfoList = Tools::getFilesPath(notesPath);
    for (auto &&fileInfo : fileInfoList) {
        NoteModel *noteModel = new NoteModel(Tools::readerFile(fileInfo.absoluteFilePath()), fileInfo.absoluteFilePath());
        g_database->addNoteText(noteModel);
    }
}

void MainWindow::setDefaultNote()
{
    if (g_configTableModel->getOpenNotesUuid().isEmpty()) {
        if (sidebarNoteList->length() != 0) {
            g_configTableModel->setOpenNotesUuid(sidebarNoteList->at(0)->getUuid());
            this->setDefaultNote();
            return;
        }
        g_noteModel = new NoteModel(Tools::readerFile(":/marked/default.md"));
    }
    else {
        g_noteModel = g_database->getNoteByUuid(g_configTableModel->getOpenNotesUuid());
    }
}

void MainWindow::setSidebarTable()
{
    ui->tableWidget_list->clear();

    sidebarNoteList = g_database->getSidebarNotes();
    ui->tableWidget_list->setRowCount(sidebarNoteList->length());
    for (int i = 0; i < sidebarNoteList->length(); ++i) {
        auto *tableWidgetitem0 = new QTableWidgetItem(sidebarNoteList->at(i)->getTitle());
        tableWidgetitem0->setData(Qt::UserRole, i);
        auto *tableWidgetitem1 = new QTableWidgetItem(
                    Tools::timestampToDateTime(sidebarNoteList->at(i)->getUpdateDate()));
        tableWidgetitem1->setData(Qt::UserRole, i);
        ui->tableWidget_list->setItem(i, 0, tableWidgetitem0);
        ui->tableWidget_list->setItem(i, 1, tableWidgetitem1);
        if (sidebarNoteList->at(i)->getUuid() == g_configTableModel->getOpenNotesUuid()) {
            ui->tableWidget_list->selectRow(i);
        }
    }

    int sidebarSortKey = g_configTableModel->getSidebarSortKey();
    Qt::SortOrder sidebarSortValue = g_configTableModel->getSidebarSortValue() == "DESC" ? Qt::DescendingOrder
                                                                                      : Qt::AscendingOrder;
    ui->tableWidget_list->horizontalHeader()->sortIndicatorChanged(sidebarSortKey, sidebarSortValue);
    ui->tableWidget_list->horizontalHeader()->setSortIndicator(sidebarSortKey, sidebarSortValue);
}

void MainWindow::on_tableWidget_list_itemClicked(QTableWidgetItem *item)
{
    qDebug() << "on_tableWidget_list_itemClicked: " << item->data(Qt::UserRole);
    QString uuid = sidebarNoteList->at(item->data(Qt::UserRole).toInt())->getUuid();
    if (g_configTableModel->getOpenNotesUuid() == uuid) {
        return;
    }
    g_configTableModel->setOpenNotesUuid(uuid);
    this->setDefaultNote();
    this->setMainWindowData();
}

void MainWindow::on_tableWidget_list_doubleClicked(const QModelIndex &index)
{

}

void MainWindow::setEditText()
{
    disconnect(ui->plainTextEdit_editor, &QPlainTextEdit::textChanged, this, &MainWindow::textChanged);
    disconnect(ui->lineEdit_title, &QLineEdit::textChanged, this, &MainWindow::textChanged);

    ui->lineEdit_title->setText(g_noteModel->noteTableModel->getTitle());
    ui->plainTextEdit_editor->setPlainText(g_noteModel->noteTableModel->getBody());

    this->setModified(true);
    this->updatePreview();

    // 监听编辑器中文本是否有更改
    connect(ui->plainTextEdit_editor, &QPlainTextEdit::textChanged, this, &MainWindow::textChanged);
    connect(ui->lineEdit_title, &QLineEdit::textChanged, this, &MainWindow::textChanged);
}

void MainWindow::onSaveFile()
{
    if (!this->isModified()) {
        return;
    }

    QFile f(g_noteModel->noteTableModel->getFilePath());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))  {
        QMessageBox::warning(this, windowTitle(),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(g_noteModel->noteTableModel->getFilePath()), f.errorString()));
        return;
    }

    QTextStream str(&f);
    str << g_noteModel->getNote();

    g_database->addNoteText(g_noteModel);
    this->setSidebarTable();

    f.close();
}

void MainWindow::onNewFile()
{
    this->onSaveFile();

    g_noteModel->clear();
    this->setMainWindowData();
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
    this->onRemoveFile();
}

void MainWindow::onRemoveFile()
{
    g_database->deleteNoteByUuid(g_noteModel->noteTableModel->getUuid());
    QFile::remove(g_noteModel->noteTableModel->getFilePath());
    g_configTableModel->setOpenNotesUuid("");
    g_noteModel->clear();

    this->setSidebarTable();
    this->setDefaultNote();
    this->setMainWindowData();
}

void MainWindow::on_headerView_sortIndicatorChanged(int logicalIndex, Qt::SortOrder order)
{
    g_configTableModel->setSidebarSortKey(logicalIndex);
    g_configTableModel->setSidebarSortValue(order == Qt::DescendingOrder ? "DESC" : "ASC");
}

void MainWindow::setTagsdata()
{
    QString tagsString;
    for (auto &&item : *g_noteModel->tagTableList) {
        tagsString += item->getName() + g_tagSplit;
    }
    tagsString.chop(g_tagSplit.length());
    ui->pushButton_tags_2->setText(tagsString);
}

void MainWindow::setMainWindowData()
{
    this->setEditText();
    this->setModified(false);

    this->setTagsdata();
    ui->pushButton_categories->setText(g_noteModel->categoriesTableModel->getName());
}

void MainWindow::onChangeCategories()
{
    ui->pushButton_categories->setText(g_noteModel->categoriesTableModel->getName());
    this->setModified(true);
    this->onSaveFile();
}

void MainWindow::on_pushButton_folder_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);

    if (ui->listWidget_categories->count() > 0) {
        return;
    }

    m_categoriesList = g_database->selectCategoriesTable();

    for (auto &&datum : m_categoriesList) {
        datum->setCount(g_database->selectNJCTableByCategoriesId(datum->getCategoriesId()).length());
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget_categories);
        ui->listWidget_categories->addItem(item);
        CategoriesListCell *categoriesListCell = new CategoriesListCell(datum);
        item->setSizeHint(categoriesListCell->sizeHint());
        ui->listWidget_categories->setItemWidget(item, categoriesListCell);
    }
}

void MainWindow::on_listWidget_categories_itemClicked(QListWidgetItem *item)
{
    QWidget *widget = ui->listWidget_categories->itemWidget(item)->findChild<QWidget *>("widget_3");

    if (item->data(Qt::UserRole).isNull()) {
        for (int i = 0; i < ui->listWidget_categories->count(); ++i) {
            QListWidgetItem *listWidgetItem = ui->listWidget_categories->item(i);
            QWidget *widget2 = ui->listWidget_categories->itemWidget(listWidgetItem)->findChild<QWidget *>("widget_3");
            widget2->setStyleSheet("QWidget#widget_3{"
                                     "background-color:#FFFFFF;"
                                     "border-width: 1px;"
                                     "border-style: solid;"
                                     "border-radius: 4px;"
                                     "border-color:#DFDFE0;}");
            listWidgetItem->setData(Qt::UserRole, QVariant());
        }

        item->setData(Qt::UserRole, 1);
        widget->setStyleSheet("QWidget#widget_3{"
                                           "background-color:#FFFFFF;"
                                           "border-width: 2px;"
                                           "border-style: solid;"
                                           "border-radius: 4px;"
                                           "border-color:#7EBFF5;}");
    }
}

void MainWindow::on_listWidget_categories_itemDoubleClicked(QListWidgetItem *item)
{
    qDebug() << "on_listWidget_categories_itemDoubleClicked";
}
