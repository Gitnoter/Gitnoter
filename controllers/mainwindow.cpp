#include "mainwindow.h"
#include "previewpage.h"
#include "database.h"
#include "ui_mainwindow.h"

#include <QWebChannel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->menuPushButtons.insert("pushButton_folder", ui->pushButton_folder);
    this->menuPushButtons.insert("pushButton_notes", ui->pushButton_notes);
    this->menuPushButtons.insert("pushButton_search", ui->pushButton_search);
    this->menuPushButtons.insert("pushButton_sync", ui->pushButton_sync);
    this->menuPushButtons.insert("pushButton_tags", ui->pushButton_tags);
    this->menuPushButtons.insert("pushButton_Trash", ui->pushButton_Trash);

    QMap<QString, QPushButton *>::Iterator itr;
    for (itr = this->menuPushButtons.begin(); itr != this->menuPushButtons.end(); ++itr) {
//        qDebug() << "key:"<<itr.key()<<"value:"<<itr.value();
        connect(itr.value(), &QPushButton::clicked, this, &MainWindow::menuPushButtonClicked);
    }

    ui->lineEdit_title->setAttribute(Qt::WA_MacShowFocusRect, 0);   // 屏蔽选中时的边框颜色

    ui->webEngineView_preview->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->webEngineView_preview->setContextMenuPolicy(Qt::NoContextMenu);

    PreviewPage *page = new PreviewPage(this);
    ui->webEngineView_preview->setPage(page);

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_content);
    page->setWebChannel(channel);

    ui->webEngineView_preview->setUrl(QUrl("qrc:/marked/index.html"));

    QFile defaultTextFile(":/marked/default.md");
    defaultTextFile.open(QIODevice::ReadOnly);
    QString defaultText = QString(defaultTextFile.readAll());

    noteModel = new NoteModel(defaultText);
    ui->lineEdit_title->setText(noteModel->getTitle());
    ui->plainTextEdit_editor->setPlainText(noteModel->getBody());

    Database *database = new Database();
    database->insertNote(noteModel);
    qDebug() << database->selectNote();
    qDebug() << noteModel->getCreateDate();

    updatePreview();

    connect(ui->plainTextEdit_editor, &QPlainTextEdit::textChanged, this, &MainWindow::textChanged);
    connect(ui->lineEdit_title, &QLineEdit::textChanged, this, &MainWindow::textChanged);

    //    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onFileNew);
    //    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
    //    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onFileSave);
    //    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    //    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onExit);

    //    connect(ui->editor->document(), &QTextDocument::modificationChanged,
    //            ui->actionSave, &QAction::setEnabled);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::textChanged()
{
    noteModel->setTitle(ui->lineEdit_title->displayText());
    noteModel->setBody(ui->plainTextEdit_editor->toPlainText());
    this->updatePreview();
}

void MainWindow::updatePreview()
{
    if (noteModel->getTitle().isEmpty()) {
        m_content.setText(noteModel->getBody());
    }
    else {
        m_content.setText("# " + noteModel->getTitle() + "\n\n" + noteModel->getBody());
    }
}

void MainWindow::menuPushButtonClicked()
{
    QString objectName = this->sender()->objectName();

    // TODO: 选择这里需要优化, 重复点击同一个按钮会出现闪烁, 之后改成, 点击替换按钮图片, 避免闪烁
    QMap<QString, QPushButton *>::Iterator itr;
    for (itr = this->menuPushButtons.begin(); itr != this->menuPushButtons.end(); ++itr) {
        if (itr.key() == objectName) {
            itr.value()->setChecked(true);
        }
        else {
            itr.value()->setChecked(false);
        }
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
