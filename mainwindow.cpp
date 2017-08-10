#include "mainwindow.h"
#include "previewpage.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMessageBox>
#include <QTextStream>
#include <QWebChannel>
#include <QScrollBar>
#include <QMapIterator>

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

    for (int i = 0; i < ui->tableWidget_list->horizontalHeader()->count(); ++i) {
        ui->tableWidget_list->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    ui->lineEdit_title->setAttribute(Qt::WA_MacShowFocusRect, 0);

    ui->webEngineView_preview->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->webEngineView_preview->setContextMenuPolicy(Qt::NoContextMenu);

    PreviewPage *page = new PreviewPage(this);
    ui->webEngineView_preview->setPage(page);

    connect(ui->plainTextEdit_editor, &QPlainTextEdit::textChanged, this, &MainWindow::textChangedAndUpdatePreview);
    connect(ui->lineEdit_title, &QLineEdit::textChanged, this, &MainWindow::textChangedAndUpdatePreview);

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_content);
    page->setWebChannel(channel);

    ui->webEngineView_preview->setUrl(QUrl("qrc:/index.html"));

    QFile defaultTextFile(":/default.md");
    defaultTextFile.open(QIODevice::ReadOnly);
    ui->plainTextEdit_editor->setPlainText(defaultTextFile.readAll());

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

void MainWindow::textChangedAndUpdatePreview()
{
    if (ui->lineEdit_title->displayText().isEmpty()) {
        m_content.setText(ui->plainTextEdit_editor->toPlainText());
    }
    else {
        m_content.setText("# " + ui->lineEdit_title->displayText() + "\n\n" + ui->plainTextEdit_editor->toPlainText());
    }
}

void MainWindow::menuPushButtonClicked()
{
    QString objectName = this->sender()->objectName();
    QMap<QString, QPushButton *>::Iterator itr;

    for (itr = this->menuPushButtons.begin(); itr != this->menuPushButtons.end(); ++itr) {
        if (itr.key() == objectName) {
            continue;
        }
        itr.value()->setChecked(false);
    }
}