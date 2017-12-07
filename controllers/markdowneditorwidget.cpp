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
        Globals::configModel->setOpenMainWindowNoteUuid("");
    }
    else {
        setOpenNote();
    }

    setupUi();
    setWindowTitle();
}

void MarkdownEditorWidget::init(NoteModel *noteModel, MainWindow *mainWindow)
{
    mMainWindow = mainWindow;
    mNoteModel = noteModel;
    setOpenNote();
    setupUi();
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
    ui->markdownPreview->setText(mNoteModel->getMarkdownHtml());
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

    bool b = sizes.length() == 0 || sizes[1] == ui->widget_navigationBar->minimumWidth();
    ui->splitter_background->setStyleSheet(b ? "QSplitter#splitter_background::handle {image: none;}" : "");
    ui->splitter_background->handle(1)->setDisabled(b);
    mMainWindow->menuBar()->getActionNavigationBar()->setChecked(!b);
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
    if (!parent()) { setGeometry(Globals::configModel->getEditorWindowRect()); }
    setSplitterSizes();
    setBackgroundSplitterSizes();

    showToolbar(Globals::configModel->getToolbarWidget());

    MenuBar *menuBar = mMainWindow->menuBar();

    connect(ui->markdownEditor->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(
            onMarkdownEditorSliderValueChanged(int)));
    connect(ui->markdownPreview->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(
            onMarkdownPreviewSliderValueChanged(int)));
    connect(ui->markdownEditor->highlighter(), SIGNAL(highlightingFinished()), this, SLOT(onNavigationBarChenged()));
    connect(ui->navigationBar, SIGNAL(positionClicked(int)), this, SLOT(onNavigationWidgetPositionClicked(int)));

    connect(ui->markdownEditor, SIGNAL(copyAvailable(bool)), menuBar->getActionCut(), SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(copyAvailable(bool)), menuBar->getActionCopy(), SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(copyAvailable(bool)), menuBar->getActionWebSearch(), SLOT(setEnabled(bool)));

    connect(ui->markdownEditor, SIGNAL(undoAvailable(bool)), menuBar->getActionUndo(), SLOT(setEnabled(bool)));
    connect(ui->markdownEditor, SIGNAL(redoAvailable(bool)), menuBar->getActionRedo(), SLOT(setEnabled(bool)));

    connect(menuBar->getActionUndo(), SIGNAL(triggered()), ui->markdownEditor, SLOT(undo()));
    connect(menuBar->getActionRedo(), SIGNAL(triggered()), ui->markdownEditor, SLOT(redo()));

    connect(menuBar->getActionCut(), SIGNAL(triggered()), ui->markdownEditor, SLOT(cut()));
    connect(menuBar->getActionCopy(), SIGNAL(triggered()), ui->markdownEditor, SLOT(copy()));
    connect(menuBar->getActionPaste(), SIGNAL(triggered()), ui->markdownEditor, SLOT(paste()));
    connect(menuBar->getActionSelectAll(), SIGNAL(triggered()), ui->markdownEditor, SLOT(selectAll()));
    connect(menuBar->getActionPasteHtml(), SIGNAL(triggered()), this, SLOT(pasteHtml()));
    connect(menuBar->getActionDeleteText(), SIGNAL(triggered()), this, SLOT(removeSelectedText()));
    connect(menuBar->getActionClearText(), SIGNAL(triggered()), this, SLOT(clearText()));

    connect(mMainWindow->menuBar(), SIGNAL(printAccepted(QPrinter *)), this, SLOT(print(QPrinter *)));

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
    QTextEditSearchWidget *textEditSearchWidget = ui->markdownEditor->searchWidget();

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
    Globals::configModel->setToolbarWidget(clicked);
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

void MarkdownEditorWidget::fullScreenEditMode()
{
    enterFullScreen();
}

void MarkdownEditorWidget::plusFontSize()
{
    QFont font = ui->markdownEditor->font();
    font.setPointSize(font.pointSize() + 1);
    ui->markdownEditor->setFont(font);

    Globals::configModel->setEditorFont(font);
}

void MarkdownEditorWidget::subtractFontSize()
{
    QFont font = ui->markdownEditor->font();

    if (font.pointSize() <= 0) {
        return;
    }

    font.setPointSize(font.pointSize() - 1);

    ui->markdownEditor->setFont(font);
    Globals::configModel->setEditorFont(font);
}

void MarkdownEditorWidget::resetFontSize()
{
    QFont font = ui->markdownEditor->font();
    font.setPointSize(Globals::editorFontSize);

    ui->markdownEditor->setFont(font);
    Globals::configModel->setEditorFont(font);
}

void MarkdownEditorWidget::enterFullScreen()
{
    (!parent() && isActiveWindow() && isFullScreen()) ? showNormal() : showFullScreen();
}

void MarkdownEditorWidget::setWindowTitle()
{
    QString windowTitle = mNoteModel->getTitle().isEmpty() ? tr("Untitled") : mNoteModel->getTitle();
    windowTitle += " - ";
    windowTitle += (mNoteModel->getCategory().isEmpty()
                    ? mMainWindow->groupTreeWidget()->topLevelItem(1)->text(0) : mNoteModel->getCategory());

    QWidget::setWindowTitle(windowTitle);

    if (mMainWindow) {
        mMainWindow->setWindowTitle(windowTitle);
    }
}

void MarkdownEditorWidget::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    if (!parent()) {
        Globals::configModel->setEditorWindowRect(geometry());
    }
}

void MarkdownEditorWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    QList<int> sizes = Globals::configModel->getEditorBackgroundSplitterSizes();
    int size1 = sizes.length() == 0 ? 20 : sizes[1];
    int size0 = ui->splitter_background->width() - size1;
    ui->splitter_background->setSizes({size0, size1});

    if (!parent()) {
        Globals::configModel->setEditorWindowRect(geometry());
    }
}
