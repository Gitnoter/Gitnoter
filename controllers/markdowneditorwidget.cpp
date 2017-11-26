#include "ui_markdowneditorwidget.h"
#include "ui_menubar.h"

#include "markdowneditorwidget.h"
#include "globals.h"

#include <QScrollBar>
#include <QMenu>
#include <QDebug>

MarkdownEditorWidget::MarkdownEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarkdownEditorWidget),
    mCategoryListWidget(new CategoryListWidget(this)),
    mNoteModel(nullptr)
{
    ui->setupUi(this);
    if (mMainWindow = qobject_cast<MainWindow *>(parent)) {
        init(Globals::configModel->getOpenNoteUuid(), mMainWindow);
    }
}

MarkdownEditorWidget::~MarkdownEditorWidget()
{
    delete ui;
}

void MarkdownEditorWidget::init(const QString &uuid, MainWindow *mainWindow)
{
    mMainWindow = mainWindow;
    mNoteModel = mMainWindow->noteListWidget()->getNoteModel(uuid);
    if (!mNoteModel) {
        if (!parent()) {
            close();
            return;
        }
        mMainWindow->stackedWidget()->setCurrentIndex(1);
        Globals::configModel->setOpenNoteUuid("");
    }
    else {
        setOpenNote();
    }
    setupUi();
}

void MarkdownEditorWidget::init(NoteModel *noteModel, MainWindow *mainWindow)
{
    mMainWindow = mainWindow;
    mNoteModel = noteModel;
    setOpenNote();
}

void MarkdownEditorWidget::appendTag(const QString &tag)
{
    if (mNoteModel->getTagList().indexOf(tag) == -1) {
        TagCellWidget *tagCellWidget = new TagCellWidget(tag, this);
        mTagCellWidgetList.append(tagCellWidget);
        ui->horizontalLayout->insertWidget(ui->horizontalLayout->count() - 1, tagCellWidget);
        setTagList();
        mMainWindow->groupTreeWidget()->append(Gitnoter::Tag, tag);
    }
}

void MarkdownEditorWidget::removeTag(QString tagName)
{
    if (tagName.isEmpty()) {
        TagCellWidget *tagCellWidget = mTagCellWidgetList[mTagCellWidgetList.length() - 1];
        mMainWindow->groupTreeWidget()->subtract(Gitnoter::Tag, tagCellWidget->getTagName());
        mTagCellWidgetList.removeOne(tagCellWidget);
        tagName = tagCellWidget->getTagName();
        delete tagCellWidget;
    }
    else {
        for (auto &&tagCellWidget : mTagCellWidgetList) {
            if (tagCellWidget->getTagName() == tagName) {
                mMainWindow->groupTreeWidget()->subtract(Gitnoter::Tag, tagCellWidget->getTagName());
                mTagCellWidgetList.removeOne(tagCellWidget);
                delete tagCellWidget;
            }
        }
    }

    setTagList();

    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();
    if (Gitnoter::Tag == type && name == tagName) {
        mMainWindow->updateView(Gitnoter::NoteListWidget);
    }
}

void MarkdownEditorWidget::changeCategory(const QString &category)
{
    const QString oldCategory = mNoteModel->getCategory();

    ui->pushButton_category->setText(category);
    mNoteModel->setCategory(category);
    mNoteModel->saveNoteDataToLocal();

    mMainWindow->updateView(Gitnoter::GroupTreeWidget);
}

void MarkdownEditorWidget::appendCategory(const QString &category)
{
    mMainWindow->groupTreeWidget()->append(Gitnoter::Category, category);
}

void MarkdownEditorWidget::modifyNote()
{
    ui->markdownPreview->setText(mNoteModel->getMarkdownHtml());
    mNoteModel->setNoteText(ui->markdownEditor->toPlainText());
    mNoteModel->saveNoteToLocal();
    mMainWindow->noteListWidget()->noteModelChanged(mNoteModel);
}

void MarkdownEditorWidget::on_pushButton_category_clicked()
{
    if (mCategoryListWidget->isHidden()) {
        mCategoryListWidget->init(mNoteModel, this);
        mCategoryListWidget->open();
    }
}

void MarkdownEditorWidget::on_splitter_editor_splitterMoved(int, int)
{
    Globals::configModel->setEditorSplitterSizes(ui->splitter_editor->sizes());
    setSplitterSizes();
}

bool MarkdownEditorWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->lineEdit_tag) {
        if (event->type() == QEvent::FocusIn) {
            ui->frame->setStyleSheet("QFrame#frame{border: 0.5px solid rgb(191, 191, 191);}");
            ui->pushButton_markdownPeview->setHidden(true);
            ui->pushButton_splitterPreview->setHidden(true);
            ui->stackedWidget->setCurrentIndex(1);
        }
        else if (event->type() == QEvent::FocusOut) {
            ui->frame->setStyleSheet("");
            ui->pushButton_markdownPeview->setHidden(false);
            ui->pushButton_splitterPreview->setHidden(false);
            ui->stackedWidget->setCurrentIndex(0);
        }
        // if lintEdit_tag is empty and press Backspace key, delete tag
        else if (event->type() == QEvent::KeyPress && ((QKeyEvent *) event)->key() == Qt::Key_Backspace) {
            if (ui->lineEdit_tag->text().isEmpty() && mTagCellWidgetList.length() > 0) {
                removeTag();
            }
        }
    }
    else if (object == ui->markdownEditor) {
        // If show edit tools
        if (mNoteModel && !mNoteModel->getIsDelete()) {
            if (event->type() == QEvent::FocusIn) {
                ui->stackedWidget->setCurrentIndex(0);
            }
            else if (event->type() == QEvent::FocusOut) {
                ui->stackedWidget->setCurrentIndex(1);
            }
        }

        // set menuBar enabled
        if (event->type() == QEvent::FocusIn) {
            Ui::MenuBar *menuBarUi = mMainWindow->menuBar()->getUi();
            menuBarUi->action_selectAll->setEnabled(true);
            menuBarUi->action_undo->setEnabled(ui->markdownEditor->document()->isUndoAvailable());
            menuBarUi->action_redo->setEnabled(ui->markdownEditor->document()->isRedoAvailable());

            const QString selectedText = ui->markdownEditor->textCursor().selectedText();
            menuBarUi->action_cut->setEnabled(!selectedText.isEmpty());
            menuBarUi->action_copy->setEnabled(!selectedText.isEmpty());
        }
        else if (event->type() == QEvent::FocusOut) {
            Ui::MenuBar *menuBarUi = mMainWindow->menuBar()->getUi();
            menuBarUi->action_undo->setEnabled(false);
            menuBarUi->action_redo->setEnabled(false);
        }
    }

    return QWidget::eventFilter(object, event);
}

void MarkdownEditorWidget::markdownEditorSliderValueChanged(int value, bool force) {
    // don't react if note text edit doesn't have the focus
    if (!ui->markdownEditor->hasFocus() && !force) {
        return;
    }

    QScrollBar *editScrollBar = ui->markdownEditor->verticalScrollBar();
    QScrollBar *viewScrollBar = ui->markdownPreview->verticalScrollBar();

    float editScrollFactor = static_cast<float>(value) / editScrollBar->maximum();
    int viewPosition =
            static_cast<int>(viewScrollBar->maximum() * editScrollFactor);

    // set the scroll position in the note text view
    viewScrollBar->setSliderPosition(viewPosition);
}

void MarkdownEditorWidget::markdownPreviewSliderValueChanged(int value, bool force) {
    // don't react if note text view doesn't have the focus
    if (!ui->markdownPreview->hasFocus() && !force) {
        return;
    }

    QScrollBar *editScrollBar = ui->markdownEditor->verticalScrollBar();
    QScrollBar *viewScrollBar = ui->markdownPreview->verticalScrollBar();

    float editScrollFactor =
            static_cast<float>(value) / viewScrollBar->maximum();

    int editPosition =
            static_cast<int>(editScrollBar->maximum() * editScrollFactor);

    // for some reason we get some int-min value here sometimes
    if (editPosition < 0) {
        return;
    }

    // set the scroll position in the note text edit
    editScrollBar->setSliderPosition(editPosition);
}

void MarkdownEditorWidget::onTagCellWidgetClicked(const QString tagName)
{
    removeTag(tagName);
}

void MarkdownEditorWidget::on_lineEdit_tag_returnPressed()
{
    if (!ui->lineEdit_tag->text().isEmpty()) {
        appendTag(ui->lineEdit_tag->text());
        ui->lineEdit_tag->clear();
    }
}

void MarkdownEditorWidget::on_markdownEditor_textChanged()
{
    modifyNote();
}

void MarkdownEditorWidget::setOpenNote()
{
    ui->markdownEditor->setText(mNoteModel->getNoteText());
    ui->markdownPreview->setText(mNoteModel->getMarkdownHtml());
    ui->pushButton_category->setText(mNoteModel->getCategory().isEmpty() ? tr("所有笔记") : mNoteModel->getCategory());
    ui->label_createTime->setText(tr("创建时间: %1").arg(mNoteModel->getCreateDateString()));
    ui->label_updateTime->setText(tr("更新时间: %1").arg(mNoteModel->getUpdateDateString()));

    // todo: 下面删除会失败, 可能是获取widget的时候就已经失败了, 目前还不知道到是怎么回事
//    for (int i = 1; i < ui->horizontalLayout->count() - 1; ++i) {
//        QLayoutItem *layoutItem = ui->horizontalLayout->itemAt(i);
//        ui->horizontalLayout->removeWidget(layoutItem->widget());
//        layoutItem->widget()->close();
//    }
    for (auto &&tagCellWidget : mTagCellWidgetList) {
        mTagCellWidgetList.removeOne(tagCellWidget);
        delete tagCellWidget;
    }

    QStringList tagModelList = mNoteModel->getTagList();
    for (auto &&item : tagModelList) {
        TagCellWidget *tagCellWidget = new TagCellWidget(item, this);
        mTagCellWidgetList.append(tagCellWidget);
        ui->horizontalLayout->insertWidget(ui->horizontalLayout->count() - 1, tagCellWidget);
    }

    if (mNoteModel->getIsDelete()) {
        ui->pushButton_category->setDisabled(true);
        ui->lineEdit_tag->setDisabled(true);
        ui->label_tagIcon->setDisabled(true);
        ui->markdownEditor->setReadOnly(true);
        ui->stackedWidget->setCurrentIndex(1);
    }
    else {
        ui->pushButton_category->setDisabled(false);
        ui->lineEdit_tag->setDisabled(false);
        ui->label_tagIcon->setDisabled(false);
        ui->markdownEditor->setReadOnly(false);
        mMainWindow->stackedWidget()->setCurrentIndex(0);
    }
}

void MarkdownEditorWidget::setTagList()
{
    QStringList tagList = {};
    for (auto &&tagCellWidget : mTagCellWidgetList) {
        tagList.append(tagCellWidget->getTagName());
    }
    mNoteModel->setTagList(tagList);
    mNoteModel->saveNoteDataToLocal();
    mMainWindow->noteListWidget()->noteModelChanged(mNoteModel);
}

void MarkdownEditorWidget::on_pushButton_splitterPreview_clicked()
{
    int width = ui->splitter_editor->size().width() / 2;
    QList<int> sizes = {width, width};
    Globals::configModel->setEditorSplitterSizes(sizes);
    setSplitterSizes();
}

void MarkdownEditorWidget::on_pushButton_markdownPeview_clicked()
{
    QList<int> sizes = {0, ui->splitter_editor->size().width()};
    Globals::configModel->setEditorSplitterSizes(sizes);
    setSplitterSizes();
}

void MarkdownEditorWidget::setSplitterSizes()
{
    QList<int> sizes = Globals::configModel->getEditorSplitterSizes();
    ui->splitter_editor->setSizes(sizes);

    bool b = sizes.indexOf(0) != -1;
    ui->splitter_editor->setStyleSheet(b ? "QSplitter#splitter_editor::handle {image: none;}" : "");
    ui->splitter_editor->handle(1)->setDisabled(b);
}

void MarkdownEditorWidget::on_markdownEditor_customContextMenuRequested(const QPoint &pos)
{
//    QTextEdit().isUndoRedoEnabled();
//    QTextEdit().textCursor().selection();
//    QPoint globalPos = ui->markdownEditor->mapToGlobal(pos);
//    QMenu *menu = ui->markdownEditor->createStandardContextMenu();

//    menu->findChild<QAction *>("edit-undo")->setText(tr("撤销"));
//    menu->findChild<QAction *>("edit-redo")->setText(tr("重做"));
//    menu->findChild<QAction *>("edit-cut")->setText(tr("剪切"));
//    menu->findChild<QAction *>("edit-copy")->setText(tr("拷贝"));
//    menu->findChild<QAction *>("edit-paste")->setText(tr("粘贴"));
//    menu->findChild<QAction *>("edit-delete")->setText(tr("删除"));
//    menu->findChild<QAction *>("select-all")->setText(tr("全选"));

//    menu->exec(globalPos);
}

void MarkdownEditorWidget::setupUi()
{
    ui->lineEdit_tag->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEdit_tag->installEventFilter(this);
    ui->markdownEditor->installEventFilter(this);

    setSplitterSizes();

    Ui::MenuBar *menuBarUi = mMainWindow->menuBar()->getUi();

    connect(ui->markdownEditor->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(markdownEditorSliderValueChanged(int)));
    connect(ui->markdownPreview->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(markdownPreviewSliderValueChanged(int)));

    connect(ui->markdownEditor, SIGNAL(copyAvailable(bool)), menuBarUi->action_cut, SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(copyAvailable(bool)), menuBarUi->action_copy, SLOT(setEnabled(bool)));

    connect(ui->markdownEditor, SIGNAL(undoAvailable(bool)), menuBarUi->action_undo, SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(redoAvailable(bool)), menuBarUi->action_redo, SLOT(setEnabled(bool)));


    connect(menuBarUi->action_undo, SIGNAL(triggered()), ui->markdownEditor, SLOT(undo()));
    connect(menuBarUi->action_redo, SIGNAL(triggered()), ui->markdownEditor, SLOT(redo()));

    connect(menuBarUi->action_cut, SIGNAL(triggered()), ui->markdownEditor, SLOT(cut()));
    connect(menuBarUi->action_copy, SIGNAL(triggered()), ui->markdownEditor, SLOT(copy()));
    connect(menuBarUi->action_paste, SIGNAL(triggered()), ui->markdownEditor, SLOT(paste()));
    connect(menuBarUi->action_selectAll, SIGNAL(triggered()), ui->markdownEditor, SLOT(selectAll()));
    connect(menuBarUi->action_deleteText, SIGNAL(triggered()), ui->markdownEditor, SLOT(clear()));

    connect(mMainWindow->menuBar(), SIGNAL(printAccepted(QPrinter *)), this, SLOT(print(QPrinter *)));

    connect(menuBarUi->action_saveNote, SIGNAL(triggered()), this, SLOT(saveNote()));
    connect(menuBarUi->action_findWithFolder, SIGNAL(triggered()), this, SLOT(openPath()));
    connect(menuBarUi->action_copyLine, SIGNAL(triggered()), this, SLOT(copyLine()));
}

void MarkdownEditorWidget::saveNote()
{
    if (mNoteModel) {
        mNoteModel->saveNoteToLocal();
    }
}

void MarkdownEditorWidget::openPath()
{
    if (mNoteModel) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(mNoteModel->getNoteDir()));
    }
}

void MarkdownEditorWidget::print(QPrinter *printer)
{

}

void MarkdownEditorWidget::copyLine()
{
    int blockNumber = ui->markdownEditor->textCursor().blockNumber();
    QTextDocument *doc = ui->markdownEditor->document();
    QTextBlock tb = doc->findBlockByLineNumber(blockNumber); // The second line.
    QClipboard *board = QApplication::clipboard();
    board->setText(tb.text());
}
