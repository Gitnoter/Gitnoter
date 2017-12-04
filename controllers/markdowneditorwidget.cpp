#include "ui_markdowneditorwidget.h"
#include "ui_menubar.h"

#include "markdowneditorwidget.h"
#include "globals.h"
#include "tools.h"

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
    mNoteModel->setNoteText(ui->markdownEditor->toPlainText());
    ui->markdownPreview->setText(mNoteModel->getMarkdownHtml());
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
            menuBarUi->action_webSearch->setEnabled(!selectedText.isEmpty());
        }
        else if (event->type() == QEvent::FocusOut) {
            Ui::MenuBar *menuBarUi = mMainWindow->menuBar()->getUi();
            menuBarUi->action_undo->setEnabled(false);
            menuBarUi->action_redo->setEnabled(false);
            menuBarUi->action_redo->setEnabled(false);
        }
    }

    return QWidget::eventFilter(object, event);
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
    ui->splitter_editor->setStyleSheet(b ? "QSplitter::handle {image: none;}" : "");
    ui->splitter_editor->handle(1)->setDisabled(b);
}

void MarkdownEditorWidget::setBackgroundSplitterSizes()
{
    QList<int> sizes = Globals::configModel->getEditorBackgroundSplitterSizes();

    ui->splitter_background->setSizes(sizes);

    bool b = sizes[1] == ui->widget_navigationBar->minimumWidth();
    ui->splitter_background->setStyleSheet(b ? "QSplitter#splitter_background::handle {image: none;}" : "");
    ui->splitter_background->handle(1)->setDisabled(b);

    Ui::MenuBar *menuBarUi = mMainWindow->menuBar()->getUi();
    menuBarUi->action_navigationBar->setChecked(!b);
}

void MarkdownEditorWidget::on_markdownEditor_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->markdownEditor->mapToGlobal(pos);
    QMenu *menu = ui->markdownEditor->createStandardContextMenu();

    menu->findChild<QAction *>("edit-undo")->setText(tr("撤销"));
    menu->findChild<QAction *>("edit-redo")->setText(tr("重做"));
    menu->findChild<QAction *>("edit-cut")->setText(tr("剪切"));
    menu->findChild<QAction *>("edit-copy")->setText(tr("拷贝"));
    menu->findChild<QAction *>("edit-paste")->setText(tr("粘贴"));
    menu->findChild<QAction *>("edit-delete")->setText(tr("删除"));
    menu->findChild<QAction *>("select-all")->setText(tr("全选"));

    menu->exec(globalPos);
}

void MarkdownEditorWidget::setupUi()
{
    ui->lineEdit_tag->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->navigationBar->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEdit_tag->installEventFilter(this);
    ui->markdownEditor->installEventFilter(this);
    ui->markdownEditor->initSearchFrame(ui->widget_searchWidget);
    setSplitterSizes();
    setBackgroundSplitterSizes();

    showToolbar(Globals::configModel->getToolbarWidget());

    Ui::MenuBar *menuBarUi = mMainWindow->menuBar()->getUi();

    connect(ui->markdownEditor->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(
            onMarkdownEditorSliderValueChanged(int)));
    connect(ui->markdownPreview->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(
            onMarkdownPreviewSliderValueChanged(int)));
    connect(ui->markdownEditor->highlighter(), SIGNAL(highlightingFinished()), this, SLOT(onNavigationBarChenged()));
    connect(ui->navigationBar, SIGNAL(positionClicked(int)), this, SLOT(onNavigationWidgetPositionClicked(int)));

    connect(ui->markdownEditor, SIGNAL(copyAvailable(bool)), menuBarUi->action_cut, SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(copyAvailable(bool)), menuBarUi->action_copy, SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(copyAvailable(bool)), menuBarUi->action_webSearch, SLOT(setEnabled(bool)));

    connect(ui->markdownEditor, SIGNAL(undoAvailable(bool)), menuBarUi->action_undo, SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(redoAvailable(bool)), menuBarUi->action_redo, SLOT(setEnabled(bool)));

    connect(menuBarUi->action_undo, SIGNAL(triggered()), ui->markdownEditor, SLOT(undo()));
    connect(menuBarUi->action_redo, SIGNAL(triggered()), ui->markdownEditor, SLOT(redo()));

    connect(menuBarUi->action_cut, SIGNAL(triggered()), ui->markdownEditor, SLOT(cut()));
    connect(menuBarUi->action_copy, SIGNAL(triggered()), ui->markdownEditor, SLOT(copy()));
    connect(menuBarUi->action_paste, SIGNAL(triggered()), ui->markdownEditor, SLOT(paste()));
    connect(menuBarUi->action_selectAll, SIGNAL(triggered()), ui->markdownEditor, SLOT(selectAll()));
    connect(menuBarUi->action_pasteHtml, SIGNAL(triggered()), this, SLOT(pasteHtml()));
    connect(menuBarUi->action_deleteText, SIGNAL(triggered()), this, SLOT(removeSelectedText()));
    connect(menuBarUi->action_clearText, SIGNAL(triggered()), this, SLOT(clearText()));

    connect(mMainWindow->menuBar(), SIGNAL(printAccepted(QPrinter *)), this, SLOT(print(QPrinter *)));

    connect(menuBarUi->action_saveNote, SIGNAL(triggered()), this, SLOT(saveNote()));
    connect(menuBarUi->action_findWithFolder, SIGNAL(triggered()), this, SLOT(openPath()));
    connect(menuBarUi->action_copyLine, SIGNAL(triggered()), this, SLOT(copyLine()));
    connect(menuBarUi->action_deleteLine, SIGNAL(triggered()), this, SLOT(removeLine()));
    connect(menuBarUi->action_webSearch, SIGNAL(triggered()), this, SLOT(webSearchText()));

    connect(menuBarUi->action_findText, SIGNAL(triggered()), this, SLOT(showSearchFindWidget()));
    connect(menuBarUi->action_replaceText, SIGNAL(triggered()), this, SLOT(showSearchReplaceWidget()));
    connect(menuBarUi->action_findNext, SIGNAL(triggered()), ui->markdownEditor->searchWidget(), SLOT(doSearchDown()));
    connect(menuBarUi->action_findLast, SIGNAL(triggered()), ui->markdownEditor->searchWidget(), SLOT(doSearchUp()));
    connect(menuBarUi->action_replaceAndNext, SIGNAL(triggered()), ui->markdownEditor->searchWidget(), SLOT(doReplace()));
    connect(menuBarUi->action_replaceAll, SIGNAL(triggered()), ui->markdownEditor->searchWidget(), SLOT(doReplaceAll()));

    connect(menuBarUi->action_toUppercase, SIGNAL(triggered()), this, SLOT(toUppercase()));
    connect(menuBarUi->action_toLowercase, SIGNAL(triggered()), this, SLOT(toLowercase()));
    connect(menuBarUi->action_toUppercaseAtFirst, SIGNAL(triggered()), this, SLOT(toUppercaseAtFirst()));

    connect(menuBarUi->action_toolbar, SIGNAL(triggered(bool)), this, SLOT(showToolbar(bool)));
    connect(menuBarUi->action_navigationBar, SIGNAL(triggered(bool)), this, SLOT(showNavigationBar(bool)));

    connect(menuBarUi->action_editMode, SIGNAL(triggered()), this, SLOT(editMode()));
    connect(menuBarUi->action_previewMode, SIGNAL(triggered()), this, SLOT(previewMode()));
    connect(menuBarUi->action_editPreviewMode, SIGNAL(triggered()), this, SLOT(editPreviewMode()));
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

void MarkdownEditorWidget::print(QPrinter *printer)
{

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
    if (mimeData->hasHtml()) {
        QString html = mimeData->html();
        // convert html tags to markdown
        html = NoteModel::htmlToMarkdown(html);

        // match image tags
        QRegularExpression re("<img.+?src=\"(.+?)\".*?>", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatchIterator i = re.globalMatch(html);

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
                html.replace(imageTag, markdownCode + "\n");
            }
        }
        // remove all html tags
        html.remove(QRegularExpression("<.+?>"));

        ui->markdownEditor->textCursor().insertText(html);
    }
    else if (mimeData->hasText()) {
        ui->markdownEditor->textCursor().insertText(mimeData->text());
    }
}

void MarkdownEditorWidget::webSearchText()
{
    const QString selectedText = ui->markdownEditor->textCursor().selectedText().trimmed();
    if (selectedText.isEmpty()) { return; }

    QDesktopServices::openUrl(QUrl(Globals::searchEngine + selectedText));
}

void MarkdownEditorWidget::showSearchFindWidget()
{
    QTextEditSearchWidget *textEditSearchWidget = ui->markdownEditor->searchWidget();
    Ui::MenuBar *menuBarUi = mMainWindow->menuBar()->getUi();

    if (textEditSearchWidget->isHidden() || textEditSearchWidget->isReplace()) {
        textEditSearchWidget->activate(false);
        menuBarUi->action_findLast->setEnabled(true);
        menuBarUi->action_findNext->setEnabled(true);
        menuBarUi->action_replaceAll->setEnabled(false);
        menuBarUi->action_replaceAndNext->setEnabled(false);
    }
    else {
        textEditSearchWidget->hide();
        menuBarUi->action_findLast->setEnabled(false);
        menuBarUi->action_findNext->setEnabled(false);
    }
}

void MarkdownEditorWidget::showSearchReplaceWidget()
{
    QTextEditSearchWidget *textEditSearchWidget = ui->markdownEditor->searchWidget();
    Ui::MenuBar *menuBarUi = mMainWindow->menuBar()->getUi();

    if (textEditSearchWidget->isHidden() || !textEditSearchWidget->isReplace()) {
        textEditSearchWidget->activate(true);
        menuBarUi->action_findLast->setEnabled(true);
        menuBarUi->action_findNext->setEnabled(true);
        menuBarUi->action_replaceAll->setEnabled(true);
        menuBarUi->action_replaceAndNext->setEnabled(true);
    }
    else {
        textEditSearchWidget->hide();
        menuBarUi->action_findLast->setEnabled(false);
        menuBarUi->action_findNext->setEnabled(false);
        menuBarUi->action_replaceAll->setEnabled(false);
        menuBarUi->action_replaceAndNext->setEnabled(false);
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
    Ui::MenuBar *menuBarUi = mMainWindow->menuBar()->getUi();
    menuBarUi->action_toolbar->setChecked(clicked);

    ui->widget_tools->setVisible(clicked);
    Globals::configModel->setToolbarWidget(clicked);
}

void MarkdownEditorWidget::showNavigationBar(bool clicked)
{
    Ui::MenuBar *menuBarUi = mMainWindow->menuBar()->getUi();
    menuBarUi->action_navigationBar->setChecked(clicked);

    int split1 = clicked ? ui->splitter_background->width() - 280 : ui->splitter_background->width();
    int split2 = clicked ? 240 : 20;
    ui->splitter_background->setSizes({split1, split2});

    on_splitter_background_splitterMoved();
}

void MarkdownEditorWidget::on_splitter_background_splitterMoved(int, int)
{
    Globals::configModel->setEditorBackgroundSplitterSizes(ui->splitter_background->sizes());
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
    Globals::configModel->setEditorSplitterSizes(sizes);
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
