#include "mainwindow.h"
#include "previewpage.h"
#include "tools.h"
#include "appconfig.h"

#include "ui_mainwindow.h"

#include <QWebChannel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    QFile defaultTextFile(":/marked/default.md");
//    defaultTextFile.open(QIODevice::ReadOnly);
//    QString defaultText = QString(defaultTextFile.readAll());
//
//    noteModel = new NoteModel(defaultText);
//
//
//    return;

    // 初始化数据
    database = new Database();

    this->initNotesToDatabases();

    sidebarNoteList = database->getSidebarNotes();
    configTableModel = new ConfigTableModel();
    configTableModel->setOpenNotesUuid("c6c71bef-3dbf-4fd4-ab3c-2a111f58fcde0");

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

    // 初始化编辑器
    ui->webEngineView_preview->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->webEngineView_preview->setContextMenuPolicy(Qt::NoContextMenu);

    PreviewPage *page = new PreviewPage(this);
    ui->webEngineView_preview->setPage(page);

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_content);
    page->setWebChannel(channel);

    ui->webEngineView_preview->setUrl(QUrl("qrc:/marked/index.html"));

    // 设置编辑器文本
    this->setEditText();
    this->setModified(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::textChanged()
{
    noteModel->noteTableModel->setTitle(ui->lineEdit_title->displayText().isEmpty()
                                        ? ui->lineEdit_title->placeholderText()
                                        : ui->lineEdit_title->displayText());
    noteModel->noteTableModel->setBody(ui->plainTextEdit_editor->toPlainText());
    noteModel->noteTableModel->setUpdateDate(0);
    this->updatePreview();
}

void MainWindow::updatePreview()
{
    if (noteModel->noteTableModel->getTitle().isEmpty()) {
        m_content.setText(noteModel->noteTableModel->getBody());
    }
    else {
        m_content.setText("# " + noteModel->noteTableModel->getTitle() + "\n\n" + noteModel->noteTableModel->getBody());
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

void MainWindow::on_pushButton_clicked()
{
    if (categoriesWidget) {
        categoriesWidget->close();
    }
    categoriesWidget = new CategoriesWidget(this);
    categoriesWidget->show();

    connect(this, &MainWindow::resizeChildWindow, categoriesWidget, &CategoriesWidget::resizeWindow);
}

void MainWindow::resizeEvent(QResizeEvent *size) {
    emit resizeChildWindow(size->size());
}

void MainWindow::initNotesToDatabases()
{
    QString notesPath = QDir(AppConfig::repoPath).filePath(AppConfig::noteFolderName);
    QFileInfoList fileInfoList = Tools::getFilesPath(notesPath);
    for (auto &&fileInfo : fileInfoList) {
        NoteModel *noteModel = new NoteModel(Tools::readerFile(fileInfo.absoluteFilePath()), fileInfo.absoluteFilePath());
        database->addNoteText(noteModel);
    }
}

void MainWindow::setDefaultNote()
{
    if (sidebarNoteList->length() == 0 && configTableModel->getOpenNotesUuid().isEmpty()) {
        noteModel = new NoteModel(Tools::readerFile(":/marked/default.md"));
    }
    else {
        noteModel = database->getNoteByUuid(configTableModel->getOpenNotesUuid());
    }
}

void MainWindow::setSidebarTable()
{
    ui->tableWidget_list->setRowCount(sidebarNoteList->length());
    for (int i = 0; i < sidebarNoteList->length(); ++i) {
        ui->tableWidget_list->setItem(i, 0, new QTableWidgetItem(sidebarNoteList->at(i)->getTitle()));
        ui->tableWidget_list->setItem(i, 1, new QTableWidgetItem(Tools::timestampToDateTime(sidebarNoteList->at(i)->getUpdateDate())));
    }
}

void MainWindow::on_tableWidget_list_doubleClicked(const QModelIndex &index)
{
    QString uuid = sidebarNoteList->at(index.row())->getUuid();
    if (configTableModel->getOpenNotesUuid() == uuid) {
        return;
    }
    configTableModel->setOpenNotesUuid(uuid);
    this->setDefaultNote();
    this->setEditText();
    this->setModified(false);
}

void MainWindow::setEditText()
{
    disconnect(ui->plainTextEdit_editor, &QPlainTextEdit::textChanged, this, &MainWindow::textChanged);
    disconnect(ui->lineEdit_title, &QLineEdit::textChanged, this, &MainWindow::textChanged);

    ui->lineEdit_title->setText(noteModel->noteTableModel->getTitle());
    ui->plainTextEdit_editor->setPlainText(noteModel->noteTableModel->getBody());

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

    QFile f(noteModel->noteTableModel->getFilePath());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))  {
        QMessageBox::warning(this, windowTitle(),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(noteModel->noteTableModel->getFilePath()), f.errorString()));
        return;
    }

    QTextStream str(&f);
    str << noteModel->getNote();

    database->addNoteText(noteModel);
    this->setSidebarTable();

    f.close();
}

void MainWindow::onNewFile()
{
    this->onSaveFile();

    noteModel->clear();
    this->setEditText();
    this->setModified(false);
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
