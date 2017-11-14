#include "ui_markdowneditorwidget.h"

#include "markdowneditorwidget.h"
#include "mainwindow.h"
#include "globals.h"

#include <QScrollBar>
#include <QDebug>

MarkdownEditorWidget::MarkdownEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarkdownEditorWidget),
    mCategoryListWidget(new CategoryListWidget(this))
{
    ui->setupUi(this);
    ui->lineEdit_tag->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEdit_tag->installEventFilter(this);
    ui->markdownEditor->installEventFilter(this);

    connect(ui->markdownEditor->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(markdownEditorSliderValueChanged(int)));
    connect(ui->markdownPreview->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(markdownPreviewSliderValueChanged(int)));
}

MarkdownEditorWidget::~MarkdownEditorWidget()
{
    delete ui;
}

void MarkdownEditorWidget::init(NoteModel *noteModel, GroupTreeWidget *groupTreeWidget, NoteListWidget *noteListWidget,
                                MainWindow *mainWindow)
{
    mNoteModel = noteModel;
    mGroupTreeWidget = groupTreeWidget;
    mNoteListWidget = noteListWidget;
    mMainWindow = mainWindow;

    ui->markdownEditor->setText(noteModel->getNoteText());
    ui->markdownPreview->setText(noteModel->getMarkdownHtml());
    ui->pushButton_category->setText(noteModel->getCategory().isEmpty() ? tr("所有笔记") : noteModel->getCategory());
    ui->label_createTime->setText(tr("创建时间: %1").arg(noteModel->getCreateDateString()));
    ui->label_updateTime->setText(tr("更新时间: %1").arg(noteModel->getUpdateDateString()));

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

    QStringList tagModelList = noteModel->getTagList();
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
    }
}

void MarkdownEditorWidget::on_splitter_editor_splitterMoved(int pos, int)
{
    setSplitterHandleDisable(0 == pos || ui->splitter_editor->size().width() == pos);
}

void MarkdownEditorWidget::setSplitterHandleDisable(bool b)
{
    ui->splitter_editor->setStyleSheet(b ? "QSplitter#splitter_editor::handle {image: none;}" : "");
    ui->splitter_editor->handle(1)->setDisabled(b);
}

void MarkdownEditorWidget::removeTag(const QString &tagName)
{
    if (tagName.isEmpty()) {
//        QLayoutItem *layoutItem = ui->horizontalLayout->itemAt(ui->horizontalLayout->count() - 2);
//        layoutItem->widget()->close();
//        ui->horizontalLayout->removeItem(layoutItem);

        TagCellWidget *tagCellWidget = mTagCellWidgetList[mTagCellWidgetList.length() - 1];
        mTagCellWidgetList.removeOne(tagCellWidget);
        delete tagCellWidget;
    }
    else {
        for (auto &&tagCellWidget : mTagCellWidgetList) {
            if (tagCellWidget->getTagName() == tagName) {
                mTagCellWidgetList.removeOne(tagCellWidget);
                delete tagCellWidget;
            }
        }
    }

    setTagList();

    if (Globals::configModel->getSideSelectedName() == tagName) {
        mMainWindow->setNoteList();
        mMainWindow->setItemSelected();
        mMainWindow->setGroupName();
        mMainWindow->setOpenNote();
    }
}

bool MarkdownEditorWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->lineEdit_tag) {
        if (event->type() == QEvent::FocusIn) {
            ui->frame->setStyleSheet("QFrame#frame{border: 0.5px solid rgb(191, 191, 191);}");
            ui->pushButton_markdownPeview->setHidden(true);
            ui->pushButton_splitterPreview->setHidden(true);
            ui->stackedWidget->setCurrentIndex(1);
            return false;
        }
        else if (event->type() == QEvent::FocusOut) {
            ui->frame->setStyleSheet("");
            ui->pushButton_markdownPeview->setHidden(false);
            ui->pushButton_splitterPreview->setHidden(false);
            ui->stackedWidget->setCurrentIndex(0);
            return false;
        }
        // if lintEdit_tag is empty and press Backspace key, delete tag
        else if (event->type() == QEvent::KeyPress && ((QKeyEvent *) event)->key() == Qt::Key_Backspace) {
            if (ui->lineEdit_tag->text().isEmpty() && mTagCellWidgetList.length() > 0) {
                removeTag();
                return false;
            }
        }
    }
    else if (object == ui->markdownEditor) {
        if (mNoteModel && !mNoteModel->getIsDelete()) {
            if (event->type() == QEvent::FocusIn) {
                ui->stackedWidget->setCurrentIndex(0);
                return false;
            }
            else if (event->type() == QEvent::FocusOut) {
                ui->stackedWidget->setCurrentIndex(1);
                return false;
            }
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
        addTag();
        ui->lineEdit_tag->clear();
    }
}

void MarkdownEditorWidget::on_markdownEditor_textChanged()
{
    ui->markdownPreview->setText(mNoteModel->getMarkdownHtml());
    mNoteModel->setNoteText(ui->markdownEditor->toPlainText());
    mNoteModel->saveNoteToLocal();
    mNoteListWidget->onNoteListWidgetChanged(mNoteModel);
}

void MarkdownEditorWidget::setTagList()
{
    QStringList tagList = {};
    for (int i = 1; i < ui->horizontalLayout->count() - 1; ++i) {
        tagList.append(((TagCellWidget *) ui->horizontalLayout->itemAt(i)->widget())->getTagName());
    }
    mNoteModel->setTagList(tagList);
    mNoteModel->saveNoteDataToLocal();
    mNoteListWidget->onNoteListWidgetChanged(mNoteModel);
}

void MarkdownEditorWidget::on_pushButton_category_clicked()
{
    if (mCategoryListWidget->isHidden()) {
        mCategoryListWidget->init(mGroupTreeWidget, mNoteModel, this);
        mCategoryListWidget->open();
    }
}

void MarkdownEditorWidget::changeCategory(const QString &category)
{
    ui->pushButton_category->setText(category);

    mNoteModel->setCategory(category);
    mNoteModel->saveNoteDataToLocal();
    mNoteListWidget->onNoteListWidgetChanged(mNoteModel);
}

void MarkdownEditorWidget::appendCategory(const QString &category)
{
    mGroupTreeWidget->append(Gitnoter::Category, category);
}

void MarkdownEditorWidget::addTag()
{
    if (mNoteModel->getTagList().indexOf(ui->lineEdit_tag->text()) == -1) {
        TagCellWidget *tagCellWidget = new TagCellWidget(ui->lineEdit_tag->text(), this);
        mTagCellWidgetList.append(tagCellWidget);
        ui->horizontalLayout->insertWidget(ui->horizontalLayout->count() - 1, tagCellWidget);
        setTagList();
        mGroupTreeWidget->append(Gitnoter::Tag, ui->lineEdit_tag->text());
    }
}
