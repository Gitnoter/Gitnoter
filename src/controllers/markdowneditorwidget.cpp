#include "ui_markdowneditorwidget.h"

#include "markdowneditorwidget.h"
#include "globals.h"
#include "tools.h"
#include "screenshot.h"

#include <QScrollBar>
#include <QFileDialog>
#include <QDebug>

MarkdownEditorWidget::MarkdownEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarkdownEditorWidget),
    mCategoryListWidget(new CategoryListWidget(this)),
    mNoteModel(nullptr)
{
    ui->setupUi(this);
    setupUi();
}

MarkdownEditorWidget::~MarkdownEditorWidget()
{
    delete ui;
}

void MarkdownEditorWidget::setupUi()
{
    ui->lineEdit_tag->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->navigationBar->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->lineEdit_tag->installEventFilter(this);
    ui->markdownEditor->installEventFilter(this);
    ui->markdownEditor->initSearchFrame(ui->widget_searchWidget);
    ui->markdownEditor->setIgnoredClickUrlSchemata({gFileScheme});

    updateMarkdownEditorSetting();
    highlightCurrentLine();
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
        gConfigModel->setOpenMainWindowNoteUuid("");
    }
    else {
        setOpenNote();
    }

    init();
    setWindowTitle();
}

void MarkdownEditorWidget::init(NoteModel *noteModel, MainWindow *mainWindow)
{
    mMainWindow = mainWindow;
    mNoteModel = noteModel;
    setOpenNote();
    init();
    setWindowTitle();
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

    Gitnoter::GroupType type = gConfigModel->getSideSelectedType();
    const QString name = gConfigModel->getSideSelectedName();
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

    setWindowTitle();
}

void MarkdownEditorWidget::appendCategory(const QString &category)
{
    mMainWindow->groupTreeWidget()->append(Gitnoter::Category, category);

    setWindowTitle();
}

void MarkdownEditorWidget::modifyNote()
{
    mNoteModel->setNoteText(ui->markdownEditor->toPlainText());
    ui->markdownPreview->setDisplayHtml(mNoteModel->getMarkdownHtml());
    mNoteModel->saveNoteToLocal();
    mMainWindow->noteListWidget()->noteModelChanged(mNoteModel);

    setWindowTitle();
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
    gConfigModel->setEditorSplitterSizes(ui->splitter_editor->sizes());
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
            mMainWindow->menuBar()->getActionSelectAll()->setEnabled(true);
            mMainWindow->menuBar()->getActionUndo()->setEnabled(ui->markdownEditor->document()->isUndoAvailable());
            mMainWindow->menuBar()->getActionRedo()->setEnabled(ui->markdownEditor->document()->isRedoAvailable());

            const QString selectedText = ui->markdownEditor->textCursor().selectedText();
            mMainWindow->menuBar()->getActionCut()->setEnabled(!selectedText.isEmpty());
            mMainWindow->menuBar()->getActionCopy()->setEnabled(!selectedText.isEmpty());
            mMainWindow->menuBar()->getActionWebSearch()->setEnabled(!selectedText.isEmpty());
        }
        else if (event->type() == QEvent::FocusOut) {
            mMainWindow->menuBar()->getActionUndo()->setEnabled(false);
            mMainWindow->menuBar()->getActionRedo()->setEnabled(false);
        }
    }

    return QWidget::eventFilter(object, event);
}

void MarkdownEditorWidget::closeEvent(QCloseEvent *) {
    mMainWindow->menuBar()->removeActionToWindowMenu(this);
}

void MarkdownEditorWidget::onMarkdownEditorSliderValueChanged(int value, bool force) {
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

void MarkdownEditorWidget::onMarkdownPreviewSliderValueChanged(int value, bool force) {
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

void MarkdownEditorWidget::onTagCellWidgetClicked(QString tagName)
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
    mMainWindow->updateAutoLockTimer();
    mMainWindow->openPurchasePanel();
}

void MarkdownEditorWidget::setOpenNote()
{
    ui->markdownEditor->setText(mNoteModel->getNoteText());
    ui->markdownPreview->setDisplayHtml(mNoteModel->getMarkdownHtml());
    ui->pushButton_category->setText(mNoteModel->getCategory().isEmpty() ? tr(u8"所有笔记") : mNoteModel->getCategory());
    ui->label_createTime->setText(tr(u8"创建时间: %1").arg(mNoteModel->getCreateDateString()));
    ui->label_updateTime->setText(tr(u8"更新时间: %1").arg(mNoteModel->getUpdateDateString()));

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
    gConfigModel->setEditorSplitterSizes(sizes);
    setSplitterSizes();
}

void MarkdownEditorWidget::on_pushButton_markdownPeview_clicked()
{
    QList<int> sizes = {0, ui->splitter_editor->size().width()};
    gConfigModel->setEditorSplitterSizes(sizes);
    setSplitterSizes();
}

void MarkdownEditorWidget::setSplitterSizes()
{
    QList<int> sizes = gConfigModel->getEditorSplitterSizes();

    ui->splitter_editor->setSizes(sizes);

    bool b = sizes.indexOf(0) != -1;
    ui->splitter_editor->handle(1)->setDisabled(b);
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
        ui->markdownEditor->setStyleSheet(b ? "" : "#markdownEditor{border: none;border-right: 0.5px solid rgb(191, 191, 191);}");
#else
    ui->splitter_editor->setStyleSheet(b ? "QSplitter::handle {image: none;}" : "");
#endif
}

void MarkdownEditorWidget::setBackgroundSplitterSizes()
{
    QList<int> sizes = gConfigModel->getEditorBackgroundSplitterSizes();

    ui->splitter_background->setSizes(sizes);
    qDebug() << sizes << ui->widget_navigationBar->width() << ui->widget_navigationBar->minimumWidth();
    bool b = sizes.length() == 0 || sizes[1] == ui->widget_navigationBar->minimumWidth();
    ui->splitter_background->handle(1)->setDisabled(b);
    mMainWindow->menuBar()->getActionNavigationBar()->setChecked(!b);
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    ui->widget_navigationBar->setStyleSheet(b ? "" : "#widget_navigationBar{border: none;border-left: 0.5px solid rgb(191, 191, 191);}");
#else
    ui->splitter_background->setStyleSheet(b ? "QSplitter#splitter_background::handle {image: none;}" : "");
#endif
}

void MarkdownEditorWidget::on_markdownEditor_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->markdownEditor->mapToGlobal(pos);
    QMenu *menu = ui->markdownEditor->createStandardContextMenu();

    menu->findChild<QAction *>("edit-undo")->setText(tr(u8"撤销"));
    menu->findChild<QAction *>("edit-redo")->setText(tr(u8"重做"));
    menu->findChild<QAction *>("edit-cut")->setText(tr(u8"剪切"));
    menu->findChild<QAction *>("edit-copy")->setText(tr(u8"拷贝"));
    menu->findChild<QAction *>("edit-paste")->setText(tr(u8"粘贴"));
    menu->findChild<QAction *>("edit-delete")->setText(tr(u8"删除"));
    menu->findChild<QAction *>("select-all")->setText(tr(u8"全选"));

    menu->exec(globalPos);
}

void MarkdownEditorWidget::init()
{
    if (!mMainWindow) {
        return;
    }

    if (!parent()) {
        setGeometry(gConfigModel->getEditorWindowRect());
    }

    setSplitterSizes();
    setBackgroundSplitterSizes();

    showToolbar(gConfigModel->getToolbarWidget());

    MenuBar *menuBar = mMainWindow->menuBar();

    connect(ui->markdownPreview->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onMarkdownPreviewSliderValueChanged(int)));
    connect(ui->navigationBar, SIGNAL(positionClicked(int)), this, SLOT(onNavigationWidgetPositionClicked(int)));

    connect(ui->markdownEditor->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onMarkdownEditorSliderValueChanged(int)));
    connect(ui->markdownEditor->highlighter(), SIGNAL(highlightingFinished()), this, SLOT(onNavigationBarChenged()));

    connect(ui->markdownEditor, SIGNAL(urlClicked(QString)), this, SLOT(onOpenLocalUrl(QString)));
    connect(ui->markdownEditor, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    connect(ui->markdownEditor, SIGNAL(copyAvailable(bool)), menuBar->getActionCut(), SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(copyAvailable(bool)), menuBar->getActionCopy(), SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(copyAvailable(bool)), menuBar->getActionWebSearch(), SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(undoAvailable(bool)), menuBar->getActionUndo(), SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(redoAvailable(bool)), menuBar->getActionRedo(), SLOT(setEnabled(bool)));

    connect(ui->pushButton_bold, SIGNAL(clicked()), this, SLOT(strong()));
    connect(ui->pushButton_headerSize, SIGNAL(clicked()), this, SLOT(headerSize()));
    connect(ui->pushButton_italic, SIGNAL(clicked()), this, SLOT(italic()));
    connect(ui->pushButton_strickout, SIGNAL(clicked()), this, SLOT(strickout()));
    connect(ui->pushButton_image, SIGNAL(clicked()), this, SLOT(linkImage()));
    connect(ui->pushButton_link, SIGNAL(clicked()), this, SLOT(link()));
    connect(ui->pushButton_quote, SIGNAL(clicked()), this, SLOT(quoteBlock()));
    connect(ui->pushButton_code, SIGNAL(clicked()), this, SLOT(codeBlock()));
    connect(ui->pushButton_ordered, SIGNAL(clicked()), this, SLOT(orderedList()));
    connect(ui->pushButton_unordered, SIGNAL(clicked()), this, SLOT(unorderedList()));
    connect(ui->pushButton_taskList, SIGNAL(clicked()), this, SLOT(todoList()));
    connect(ui->pushButton_table, SIGNAL(clicked()), this, SLOT(table()));

    connect(menuBar->getActionExportHtmlForPDF(), SIGNAL(triggered()), this, SLOT(exportHtmlForPDF()));
    connect(menuBar->getActionExportMarkdownForPDF(), SIGNAL(triggered()), this, SLOT(exportMarkdownForPDF()));
    connect(menuBar->getActionExportHtml(), SIGNAL(triggered()), this, SLOT(exportHtml()));
    connect(menuBar->getActionExportMarkdown(), SIGNAL(triggered()), this, SLOT(exportMarkdown()));

    connect(menuBar->getActionUndo(), SIGNAL(triggered()), ui->markdownEditor, SLOT(undo()));
    connect(menuBar->getActionRedo(), SIGNAL(triggered()), ui->markdownEditor, SLOT(redo()));

    connect(menuBar->getActionCut(), SIGNAL(triggered()), ui->markdownEditor, SLOT(cut()));
    connect(menuBar->getActionCopy(), SIGNAL(triggered()), ui->markdownEditor, SLOT(copy()));
    connect(menuBar->getActionPaste(), SIGNAL(triggered()), ui->markdownEditor, SLOT(paste()));
    connect(menuBar->getActionSelectAll(), SIGNAL(triggered()), ui->markdownEditor, SLOT(selectAll()));
    connect(menuBar->getActionPaste(), SIGNAL(triggered()), this, SLOT(pasteData()));
    connect(menuBar->getActionPasteHtml(), SIGNAL(triggered()), this, SLOT(pasteHtml()));
    connect(menuBar->getActionDeleteText(), SIGNAL(triggered()), this, SLOT(removeSelectedText()));
    connect(menuBar->getActionClearText(), SIGNAL(triggered()), this, SLOT(clearText()));

    connect(menuBar, SIGNAL(printEditAccepted(QPrinter *)), this, SLOT(printEdit(QPrinter * )));
    connect(menuBar, SIGNAL(printPreviewAccepted(QPrinter *)), this, SLOT(printPreview(QPrinter * )));

    connect(menuBar->getActionSaveNote(), SIGNAL(triggered()), this, SLOT(saveNote()));
    connect(menuBar->getActionFindWithFolder(), SIGNAL(triggered()), this, SLOT(openPath()));
    connect(menuBar->getActionCopyLine(), SIGNAL(triggered()), this, SLOT(copyLine()));
    connect(menuBar->getActionDeleteLine(), SIGNAL(triggered()), this, SLOT(removeLine()));
    connect(menuBar->getActionWebSearch(), SIGNAL(triggered()), this, SLOT(webSearchText()));

    connect(menuBar->getActionFindText(), SIGNAL(triggered()), this, SLOT(showSearchFindWidget()));
    connect(menuBar->getActionReplaceText(), SIGNAL(triggered()), this, SLOT(showSearchReplaceWidget()));
    connect(menuBar->getActionFindNext(), SIGNAL(triggered()), ui->markdownEditor->searchWidget(), SLOT(doSearchDown()));
    connect(menuBar->getActionFindLast(), SIGNAL(triggered()), ui->markdownEditor->searchWidget(), SLOT(doSearchUp()));
    connect(menuBar->getActionReplaceAndNext(), SIGNAL(triggered()), ui->markdownEditor->searchWidget(), SLOT(doReplace()));
    connect(menuBar->getActionReplaceAll(), SIGNAL(triggered()), ui->markdownEditor->searchWidget(), SLOT(doReplaceAll()));

    connect(menuBar->getActionToUppercase(), SIGNAL(triggered()), this, SLOT(toUppercase()));
    connect(menuBar->getActionToLowercase(), SIGNAL(triggered()), this, SLOT(toLowercase()));
    connect(menuBar->getActionToUppercaseAtFirst(), SIGNAL(triggered()), this, SLOT(toUppercaseAtFirst()));

    connect(menuBar->getActionToolbar(), SIGNAL(triggered(bool)), this, SLOT(showToolbar(bool)));
    connect(menuBar->getActionNavigationBar(), SIGNAL(triggered(bool)), this, SLOT(showNavigationBar(bool)));

    connect(menuBar->getActionEditMode(), SIGNAL(triggered()), this, SLOT(editMode()));
    connect(menuBar->getActionPreviewMode(), SIGNAL(triggered()), this, SLOT(previewMode()));
    connect(menuBar->getActionEditPreviewMode(), SIGNAL(triggered()), this, SLOT(editPreviewMode()));
    connect(menuBar->getActionFullScreenEditMode(), SIGNAL(triggered()), this, SLOT(fullScreenEditMode()));

    connect(menuBar->getActionPlusFontSize(), SIGNAL(triggered()), this, SLOT(plusFontSize()));
    connect(menuBar->getActionSubtractFontSize(), SIGNAL(triggered()), this, SLOT(subtractFontSize()));
    connect(menuBar->getActionResetFontSize(), SIGNAL(triggered()), this, SLOT(resetFontSize()));

    connect(menuBar->getActionEnterFullScreen(), SIGNAL(triggered()), this, SLOT(enterFullScreen()));

    connect(menuBar->getActionMixWindow(), SIGNAL(triggered()), this, SLOT(showMinimized()));
    connect(menuBar->getActionResizeWindow(), SIGNAL(triggered()), this, SLOT(showMaximized()));

    connect(menuBar->getActionTitle1(), SIGNAL(triggered()), this, SLOT(title1()));
    connect(menuBar->getActionTitle2(), SIGNAL(triggered()), this, SLOT(title2()));
    connect(menuBar->getActionTitle3(), SIGNAL(triggered()), this, SLOT(title3()));
    connect(menuBar->getActionTitle4(), SIGNAL(triggered()), this, SLOT(Title4()));
    connect(menuBar->getActionTitle5(), SIGNAL(triggered()), this, SLOT(title5()));
    connect(menuBar->getActionStrong(), SIGNAL(triggered()), this, SLOT(strong()));
    connect(menuBar->getActionItalic(), SIGNAL(triggered()), this, SLOT(italic()));
    connect(menuBar->getActionStrickout(), SIGNAL(triggered()), this, SLOT(strickout()));
    connect(menuBar->getActionUnderline(), SIGNAL(triggered()), this, SLOT(underline()));
    connect(menuBar->getActionTable(), SIGNAL(triggered()), this, SLOT(table()));
    connect(menuBar->getActionOrderedList(), SIGNAL(triggered()), this, SLOT(orderedList()));
    connect(menuBar->getActionUnorderedList(), SIGNAL(triggered()), this, SLOT(unorderedList()));
    connect(menuBar->getActionTaskList(), SIGNAL(triggered()), this, SLOT(todoList()));
    connect(menuBar->getActionLink(), SIGNAL(triggered()), this, SLOT(link()));
    connect(menuBar->getActionLinkImage(), SIGNAL(triggered()), this, SLOT(linkImage()));
    connect(menuBar->getActionInsertImage(), SIGNAL(triggered()), this, SLOT(insertImage()));
    connect(menuBar->getActionAccessory(), SIGNAL(triggered()), this, SLOT(accessory()));
    connect(menuBar->getActionQuoteBlock(), SIGNAL(triggered()), this, SLOT(quoteBlock()));
    connect(menuBar->getActionCuttingLine(), SIGNAL(triggered()), this, SLOT(cuttingLine()));
    connect(menuBar->getActionNowTime(), SIGNAL(triggered()), this, SLOT(nowTime()));
    connect(menuBar->getActionInnerCodeBlock(), SIGNAL(triggered()), this, SLOT(innerCodeBlock()));
    connect(menuBar->getActionCodeBlock(), SIGNAL(triggered()), this, SLOT(codeBlock()));
    connect(menuBar->getActionAnnotation(), SIGNAL(triggered()), this, SLOT(annotation()));
    connect(menuBar->getActionMoreAnnotation(), SIGNAL(triggered()), this, SLOT(moreAnnotation()));

    connect(mMainWindow->settingDialog(), &SettingDialog::editorFontChanged, [=](const QFont &font) {
        ui->markdownEditor->setFont(font);
    });
}

void MarkdownEditorWidget::saveNote()
{
    if (!mNoteModel) { return; }

    mNoteModel->saveNoteToLocal();
}

void MarkdownEditorWidget::openPath()
{
    if (!mNoteModel) { return; }

    QDesktopServices::openUrl(QUrl::fromLocalFile(mNoteModel->getNoteDir()));
}

void MarkdownEditorWidget::printEdit(QPrinter *printer)
{
    ui->markdownEditor->document()->print(printer);
}

void MarkdownEditorWidget::printPreview(QPrinter *printer)
{
    ui->markdownPreview->document()->print(printer);
}

void MarkdownEditorWidget::copyLine()
{
    if (!mNoteModel) { return; }

    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.movePosition(QTextCursor::EndOfBlock);

    const QString lineText = ui->markdownEditor->document()->findBlockByLineNumber(textCursor.blockNumber()).text();
    textCursor.insertText(QString("\n%1").arg(lineText));

    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::removeLine()
{
    if (!mNoteModel) { return; }

    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.select(QTextCursor::BlockUnderCursor);
    textCursor.removeSelectedText();
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::removeSelectedText()
{
    if (!mNoteModel) { return; }
    ui->markdownEditor->textCursor().removeSelectedText();
//
//    QTextCursor textCursor = ui->markdownEditor->textCursor();
//    textCursor.removeSelectedText();
//    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::clearText()
{
    ui->markdownEditor->selectAll();
    ui->markdownEditor->textCursor().removeSelectedText();
}

void MarkdownEditorWidget::pasteHtml()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData * mimeData = clipboard->mimeData(QClipboard::Clipboard);
    insertMimeData(mimeData);
}

void MarkdownEditorWidget::webSearchText()
{
    const QString selectedText = ui->markdownEditor->textCursor().selectedText().trimmed();
    if (selectedText.isEmpty()) { return; }

    QDesktopServices::openUrl(QUrl(gSearchEngine + selectedText));
}

void MarkdownEditorWidget::showSearchFindWidget()
{
    QPlainTextEditSearchWidget *textEditSearchWidget = ui->markdownEditor->searchWidget();

    if (textEditSearchWidget->isHidden() || textEditSearchWidget->isReplace()) {
        textEditSearchWidget->activate(false);
        mMainWindow->menuBar()->getActionFindLast()->setEnabled(true);
        mMainWindow->menuBar()->getActionFindNext()->setEnabled(true);
        mMainWindow->menuBar()->getActionReplaceAll()->setEnabled(false);
        mMainWindow->menuBar()->getActionReplaceAndNext()->setEnabled(false);
    }
    else {
        textEditSearchWidget->hide();
        mMainWindow->menuBar()->getActionFindLast()->setEnabled(false);
        mMainWindow->menuBar()->getActionFindNext()->setEnabled(false);
    }
}

void MarkdownEditorWidget::showSearchReplaceWidget()
{
    QPlainTextEditSearchWidget *textEditSearchWidget = ui->markdownEditor->searchWidget();

    if (textEditSearchWidget->isHidden() || !textEditSearchWidget->isReplace()) {
        textEditSearchWidget->activate(true);
        mMainWindow->menuBar()->getActionFindLast()->setEnabled(true);
        mMainWindow->menuBar()->getActionFindNext()->setEnabled(true);
        mMainWindow->menuBar()->getActionReplaceAll()->setEnabled(true);
        mMainWindow->menuBar()->getActionReplaceAndNext()->setEnabled(true);
    }
    else {
        textEditSearchWidget->hide();
        mMainWindow->menuBar()->getActionFindLast()->setEnabled(false);
        mMainWindow->menuBar()->getActionFindNext()->setEnabled(false);
        mMainWindow->menuBar()->getActionReplaceAll()->setEnabled(false);
        mMainWindow->menuBar()->getActionReplaceAndNext()->setEnabled(false);
    }
}

void MarkdownEditorWidget::toUppercase()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    QString selectedText = textCursor.selectedText();

    if (selectedText.isEmpty()) {
        return;
    }

    textCursor.removeSelectedText();
    textCursor.insertText(selectedText.toUpper());

    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::toLowercase()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    QString selectedText = textCursor.selectedText();

    if (selectedText.isEmpty()) {
        return;
    }

    textCursor.removeSelectedText();
    textCursor.insertText(selectedText.toLower());

    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::toUppercaseAtFirst()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    QString selectedText = textCursor.selectedText();

    if (selectedText.isEmpty()) {
        return;
    }

    textCursor.removeSelectedText();
    textCursor.insertText(Tools::firstUpperCase(selectedText));

    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::showToolbar(bool clicked)
{
    mMainWindow->menuBar()->getActionToolbar()->setChecked(clicked);

    ui->widget_tools->setVisible(clicked);
    gConfigModel->setToolbarWidget(clicked);
}

void MarkdownEditorWidget::showNavigationBar(bool clicked)
{
    mMainWindow->menuBar()->getActionNavigationBar()->setChecked(clicked);

    int split1 = clicked ? ui->splitter_background->width() - 280 : ui->splitter_background->width();
    int split2 = clicked ? 240 : 20;
    ui->splitter_background->setSizes({split1, split2});

    on_splitter_background_splitterMoved();
}

void MarkdownEditorWidget::on_splitter_background_splitterMoved(int, int)
{
    gConfigModel->setEditorBackgroundSplitterSizes(ui->splitter_background->sizes());
    setBackgroundSplitterSizes();
}

void MarkdownEditorWidget::onNavigationBarChenged()
{
    ui->navigationBar->init(ui->markdownEditor->document());
}

void MarkdownEditorWidget::onNavigationWidgetPositionClicked(int position) {
    ui->markdownEditor->setFocus();

    QTextCursor textCursor = ui->markdownEditor->textCursor();

    // if the current position of the cursor is smaller than the position
    // where we want to jump to set the cursor to the end of the note to make
    // sure it scrolls up, not down
    // everything is visible that way
    if (textCursor.position() < position) {
        textCursor.movePosition(QTextCursor::End);
        ui->markdownEditor->setTextCursor(textCursor);
    }

    textCursor.setPosition(position);

    // select the text of the headline
    textCursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

    ui->markdownEditor->setTextCursor(textCursor);

    // update the preview-slider
    onMarkdownEditorSliderValueChanged(ui->markdownEditor->verticalScrollBar()->value(), true);
}

void MarkdownEditorWidget::editMode()
{
    QList<int> sizes = {ui->splitter_editor->size().width(), 0};
    gConfigModel->setEditorSplitterSizes(sizes);
    setSplitterSizes();
}

void MarkdownEditorWidget::previewMode()
{
    on_pushButton_markdownPeview_clicked();
}

void MarkdownEditorWidget::editPreviewMode()
{
    on_pushButton_splitterPreview_clicked();
}

void MarkdownEditorWidget::fullScreenEditMode()
{
    enterFullScreen();
}

void MarkdownEditorWidget::plusFontSize()
{
    QFont font = ui->markdownEditor->font();
    font.setPointSize(font.pointSize() + 1);
    ui->markdownEditor->setFont(font);

    gConfigModel->setEditorFont(font);
}

void MarkdownEditorWidget::subtractFontSize()
{
    QFont font = ui->markdownEditor->font();

    if (font.pointSize() <= 0) {
        return;
    }

    font.setPointSize(font.pointSize() - 1);

    ui->markdownEditor->setFont(font);
    gConfigModel->setEditorFont(font);
}

void MarkdownEditorWidget::resetFontSize()
{
    QFont font = ui->markdownEditor->font();
    font.setPointSize(gEditorFontSize);

    ui->markdownEditor->setFont(font);
    gConfigModel->setEditorFont(font);
}

void MarkdownEditorWidget::enterFullScreen()
{
    (!parent() && isActiveWindow() && isFullScreen()) ? showNormal() : showFullScreen();
}

void MarkdownEditorWidget::setWindowTitle()
{
    if (!mNoteModel) { return; }
    QString windowTitle = mNoteModel->getTitle().isEmpty() ? tr(u8"未命名") : mNoteModel->getTitle();
    windowTitle += " - ";
    windowTitle += (mNoteModel->getCategory().isEmpty()
                    ? mMainWindow->groupTreeWidget()->topLevelItem(Gitnoter::All)->text(0) : mNoteModel->getCategory());

    QWidget::setWindowTitle(windowTitle);

    if (mMainWindow) {
        mMainWindow->setWindowTitle(windowTitle);
    }
}

void MarkdownEditorWidget::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    if (!parent()) {
        gConfigModel->setEditorWindowRect(geometry());
    }
}

void MarkdownEditorWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    QList<int> sizes = gConfigModel->getEditorBackgroundSplitterSizes();
    int size1 = sizes.length() == 0 ? 20 : sizes[1];
    int size0 = ui->splitter_background->width() - size1;
    ui->splitter_background->setSizes({size0, size1});

    if (!parent()) {
        gConfigModel->setEditorWindowRect(geometry());
    }
}

void MarkdownEditorWidget::exportHtmlForPDF()
{
    const QString fileName = getExportFileName(tr(u8"导出 HTML 为 PDF"), "PDF", {"pdf"});

    if (fileName.isEmpty()) {
        return;
    }

    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(fileName);

    ui->markdownPreview->document()->print(printer);
}

void MarkdownEditorWidget::exportMarkdownForPDF()
{
    const QString fileName = getExportFileName(tr(u8"导出 Markdown 为 PDF"), "PDF", {"pdf"});

    if (fileName.isEmpty()) {
        return;
    }

    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(fileName);

    ui->markdownEditor->document()->print(printer);
}

void MarkdownEditorWidget::exportHtml()
{
    const QString fileName = getExportFileName(tr(u8"导出 HTML 文本"), "HTML", {"html"});

    if (fileName.isEmpty()) {
        return;
    }

    Tools::writerFile(fileName, mNoteModel->getMarkdownHtml().toUtf8());
}

void MarkdownEditorWidget::exportMarkdown()
{
    const QString fileName = getExportFileName(tr(u8"导出 Markdown 文件"), "Markdown", {"md"});

    if (fileName.isEmpty()) {
        return;
    }

    Tools::writerFile(fileName, mNoteModel->getNoteText().toUtf8());
}

QString MarkdownEditorWidget::getExportFileName(
        const QString &windowTitle, const QString &fileType, QStringList suffixList,
        QFileDialog::AcceptMode acceptMode, QFileDialog::FileMode fileMode)
{
    QFileDialog *dialog = new QFileDialog();
    dialog->setFileMode(fileMode);
    dialog->setAcceptMode(acceptMode);
    dialog->setWindowTitle(windowTitle);

    if (suffixList.length() != 0) {
        for (int i = 0; i < suffixList.length(); ++i) {
            suffixList.replace(i, "*." + suffixList[i]);
        }
        dialog->setNameFilter(fileType + tr(" files") + " (" + suffixList.join(" ") + ")");
    }

    if (QFileDialog::AcceptSave == acceptMode) {
        dialog->selectFile(mNoteModel->getTitle());
    }

    if (dialog->exec() != QDialog::Accepted) {
        return "";
    }

    if (!dialog->selectedFiles().length()) {
        return "";
    }

    QString fileName = dialog->selectedFiles().at(0);

    if (fileName.isEmpty()) {
        return "";
    }

    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName.append("." + suffixList[0]);
    }

    return fileName;
}

void MarkdownEditorWidget::title1()
{
    if (ui->stackedWidget->currentIndex() != 0) { return; }

    setLineTextTitleSign("#");
}

void MarkdownEditorWidget::title2()
{
    if (ui->stackedWidget->currentIndex() != 0) { return; }

    setLineTextTitleSign("##");
}

void MarkdownEditorWidget::title3()
{
    if (ui->stackedWidget->currentIndex() != 0) { return; }

    setLineTextTitleSign("###");
}

void MarkdownEditorWidget::Title4()
{
    if (ui->stackedWidget->currentIndex() != 0) { return; }

    setLineTextTitleSign("####");
}

void MarkdownEditorWidget::title5()
{
    if (ui->stackedWidget->currentIndex() != 0) { return; }

    setLineTextTitleSign("#####");
}

void MarkdownEditorWidget::strong()
{
    applyFormatter("**", "**");
}

void MarkdownEditorWidget::italic()
{
    applyFormatter("*", "*");
}

void MarkdownEditorWidget::strickout()
{
    applyFormatter("~~", "~~");
}

void MarkdownEditorWidget::underline()
{
    applyFormatter("_", "_");
}

void MarkdownEditorWidget::table()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.select(QTextCursor::LineUnderCursor);
    QString text = textCursor.selectedText();
    textCursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    textCursor.insertText(QString(text.isEmpty() ? "" : "\n") + "|  |  |  |\n|---|---|---|\n|  |  |  |\n|  |  |  |\n");
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::orderedList()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.select(QTextCursor::LineUnderCursor);
    QString text = textCursor.selectedText();
    textCursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    textCursor.insertText(QString(text.isEmpty() ? "" : "\n") + "1. ");
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::unorderedList()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.select(QTextCursor::LineUnderCursor);
    QString text = textCursor.selectedText();
    textCursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    textCursor.insertText(QString(text.isEmpty() ? "" : "\n") + "* ");
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::todoList()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.select(QTextCursor::LineUnderCursor);
    QString text = textCursor.selectedText();
    textCursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    textCursor.insertText(QString(text.isEmpty() ? "" : "\n") + "- [x] ");
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::link()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.insertText("[]()");
    textCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::linkImage()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.insertText("![]()");
    textCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::insertImage()
{
    if (!ui->markdownEditor->hasFocus()) {
        return;
    }

    const QString filePath = getExportFileName(tr(u8"插入图片"), tr(u8"图片"), {"jpg", "jpeg", "png", "gif"},
                                               QFileDialog::AcceptOpen);

    if (filePath.isEmpty()) {
        return;
    }

    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.insertText(mNoteModel->saveMediaToLocal(filePath));
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::accessory()
{
    if (!ui->markdownEditor->hasFocus()) {
        return;
    }

    const QString filePath = getExportFileName(tr(u8"插入附件"), tr(u8"附件"), {},
                                               QFileDialog::AcceptOpen, QFileDialog::ExistingFile);

    if (filePath.isEmpty()) {
        return;
    }

    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.insertText(mNoteModel->saveMediaToLocal(filePath));
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::quoteBlock()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.movePosition(QTextCursor::StartOfLine);
    textCursor.insertText("> ");
    textCursor.movePosition(QTextCursor::EndOfLine);
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::cuttingLine()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.movePosition(QTextCursor::EndOfLine);
    textCursor.insertText("\n---\n");
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::nowTime()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.insertText(Tools::timestampToDateTime(static_cast<int>(QDateTime::currentSecsSinceEpoch())));
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::innerCodeBlock()
{
    applyFormatter("`", "`");
}

void MarkdownEditorWidget::codeBlock()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.insertText("```\n```");
    textCursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::annotation()
{
    applyFormatter("<!-- ", " -->");
}

void MarkdownEditorWidget::moreAnnotation()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.insertText("<!-- more -->");
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::setLineTextTitleSign(const QString &titleSign)
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.select(QTextCursor::LineUnderCursor);
    QString text = textCursor.selectedText();
    text = titleSign + " " + text.replace(QRegularExpression("^[#]*[ ]*"), "");
    textCursor.insertText(text);
    ui->markdownEditor->setTextCursor(textCursor);
}

bool MarkdownEditorWidget::undoFormatting(const QString &formatterStart, const QString &formatterEnd)
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    QString selectedText = textCursor.selectedText();
    int selectionStart = textCursor.selectionStart();
    int selectionEnd = textCursor.selectionEnd();

    textCursor.setPosition(selectionStart - formatterStart.length());
    textCursor.setPosition(selectionEnd + formatterEnd.length(), QTextCursor::KeepAnchor);
    QString selectedTextWithFormatter = textCursor.selectedText();

    // if the formatter characters were found we remove them
    if (selectedTextWithFormatter.startsWith(formatterStart) &&
        selectedTextWithFormatter.endsWith(formatterEnd)) {
        textCursor.insertText(selectedText);
        return true;
    }

    return false;
}

void MarkdownEditorWidget::applyFormatter(const QString &formatterStart, const QString &formatterEnd)
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    QString selectedText = textCursor.selectedText();

    // first try to undo an existing formatting
    if (undoFormatting(formatterStart, formatterEnd)) {
        return;
    }

    if (selectedText.isEmpty()) {
        textCursor.insertText(formatterStart + formatterEnd);
        textCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, formatterStart.length());
        ui->markdownEditor->setTextCursor(textCursor);
    } else {
        QRegularExpressionMatch match = QRegularExpression(R"(^(\s*)(.+?)(\s*)$)").match(selectedText);
        if (match.hasMatch()) {
            textCursor.insertText(match.captured(1) + formatterStart + match.captured(2) + formatterEnd + match.captured(3));
        }
    }
}

void MarkdownEditorWidget::onOpenLocalUrl(QString urlString) {
    ui->markdownEditor->openUrl(mNoteModel->getFilePath(urlString));
}

void MarkdownEditorWidget::on_markdownPreview_anchorClicked(const QUrl &arg1)
{
    qDebug() << __func__ << " - 'url': " << arg1;

    if (arg1.scheme() == gFileScheme) {
        onOpenLocalUrl(arg1.toString());
    }
    else {
        ui->markdownEditor->openUrl(arg1.toString());
    }
}

void MarkdownEditorWidget::headerSize()
{
    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.select(QTextCursor::LineUnderCursor);
    QString text = textCursor.selectedText();

    QRegularExpression rx("^#+");
    QRegularExpressionMatch rm = rx.match(text);
    if (rm.capturedTexts().length()) {
        if (rm.capturedTexts().at(0).length() >= 5) {
            text = text.replace(QRegularExpression("^[#]*"), "");
        }
        text = "#" + text;
    }
    else {
        text = "# " + text;
    }

    textCursor.insertText(text);
    ui->markdownEditor->setTextCursor(textCursor);
}

void MarkdownEditorWidget::fullScreenShot()
{
    if (!ui->markdownEditor->hasFocus()) {
        return;
    }


    qDebug() << __func__;

    const QPixmap pixmap = ScreenShot::fullScreenShot();
    savePixmap(pixmap, tr(u8"全屏"));
}

void MarkdownEditorWidget::windowScreenShot()
{
    if (!ui->markdownEditor->hasFocus()) {
        return;
    }
    qDebug() << __func__;
    QPixmap pixmap = ScreenShot::windowScreenShot();
    if (!pixmap.isNull()) {
        savePixmap(pixmap, tr(u8"窗口"));
    }
}

void MarkdownEditorWidget::partScreenShot()
{
    if (!ui->markdownEditor->hasFocus() || !mMainWindow || !isActiveWindow()) {
        return;
    }
    qDebug() << __func__;
    ScreenShot *screenShot = new ScreenShot(this);

    if (screenShot->exec() == QDialog::Accepted) {
        const QPixmap pixmap = screenShot->shotPixmap();
        if (!pixmap.isNull()) {
            savePixmap(pixmap, tr(u8"截屏"));
        }
    }

    raise();
    activateWindow();
}

void MarkdownEditorWidget::savePixmap(const QPixmap &pixmap, const QString &name)
{
    const QString filePath = QDir(mNoteModel->getNoteDir()).filePath(Tools::getShortUuid() + ".jpg");
    const QFileInfo fileInfo = QFileInfo(filePath);

    pixmap.save(filePath, "JPG", gImageQuality);

    QTextCursor textCursor = ui->markdownEditor->textCursor();
    textCursor.insertText("![" + name + "-" + fileInfo.baseName() + "](" + gFileScheme + "://" + fileInfo.fileName() + ")");
    ui->markdownEditor->setTextCursor(textCursor);
}

bool MarkdownEditorWidget::editorHasFocus()
{
    return ui->markdownEditor->hasFocus();
}


void MarkdownEditorWidget::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MarkdownEditorWidget::dropEvent(QDropEvent *event)
{
    insertMimeData(event->mimeData());
}

void MarkdownEditorWidget::insertMimeData(const QMimeData *mimeData)
{
    qDebug() << __func__ << mimeData;
    QString insertText = "";

    if (mimeData->hasImage()) {
        qDebug() << mimeData->imageData();
        QImage image = mimeData->imageData().value<QImage>();
        if (!image.isNull()) {
            insertText = mNoteModel->saveMediaToLocal(image);
        }
    }
    else if (mimeData->hasUrls()) {
        for (auto &&url : mimeData->urls()) {
            if (QFileInfo(url.toLocalFile()).isReadable()) {
                insertText += mNoteModel->saveMediaToLocal(url.toString()) + "\n";
            }
        }
    }
    else if (mimeData->hasHtml()) {
        insertText = mimeData->html();
        // convert html tags to markdown
        insertText = NoteModel::htmlToMarkdown(insertText);

        // match image tags
        QRegularExpression re("<img.+?src=\"(.+?)\".*?>", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatchIterator i = re.globalMatch(insertText);

        // find, download locally and replace all images
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString imageTag = match.captured(0);
            QString imageUrl = match.captured(1);

            qDebug() << __func__ << " - 'imageUrl': " << imageUrl;

            if (!QUrl(imageUrl).isValid()) {
                continue;
            }

            // download the image and get the media markdown code for it
            QString markdownCode = NoteModel::downloadUrlToMedia(imageUrl, mNoteModel);

            if (!markdownCode.isEmpty()) {
                // replace the image tag with markdown code
                insertText.replace(imageTag, markdownCode + "\n");
            }
        }
        // remove all html tags
        insertText.remove(QRegularExpression("<.+?>"));
    }
    else if (mimeData->hasText()) {
        insertText = mimeData->text();
    }

    ui->markdownEditor->textCursor().insertText(insertText);
}

void MarkdownEditorWidget::pasteData()
{
    qDebug() << __func__;
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData * mimeData = clipboard->mimeData(QClipboard::Clipboard);
    if (mimeData->hasHtml() || mimeData->hasText()) {
        return;
    }

    insertMimeData(mimeData);
}

void MarkdownEditorWidget::updateMarkdownEditorSetting()
{
    QMarkdownTextEdit::AutoTextOptions options;
    options |= QMarkdownTextEdit::AutoTextOption::BracketClosing;
    options |= QMarkdownTextEdit::AutoTextOption::BracketRemoval;

    ui->markdownEditor->setAutoTextOptions(options);
}

void MarkdownEditorWidget::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    // check if current line is really visible!
    if (!ui->markdownEditor->isReadOnly()) {
        ui->markdownEditor->ensureCursorVisible();
        QTextEdit::ExtraSelection selection = QTextEdit::ExtraSelection();

        QColor lineColor = QColor(255, 250, 225);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = ui->markdownEditor->textCursor();
        //        selection.cursor.clearSelection();
        //        selection.cursor.select(QTextCursor::BlockUnderCursor);
        extraSelections.append(selection);
    }

    // be aware that extra selections, like for global searching, gets
    // removed when the current line gets highlighted
    ui->markdownEditor->setExtraSelections(extraSelections);
}
