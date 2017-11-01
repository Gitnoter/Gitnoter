#include "ui_markdowneditorwidget.h"

#include "markdowneditorwidget.h"
#include "globals.h"

#include <QScrollBar>
#include <QDebug>

MarkdownEditorWidget::MarkdownEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarkdownEditorWidget)
{
    ui->setupUi(this);
    ui->lineEdit_tag->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEdit_tag->installEventFilter(this);

    connect(ui->markdownEditor->verticalScrollBar(), SIGNAL(valueChanged(int)),
                this, SLOT(markdownEditorSliderValueChanged(int)));
    connect(ui->markdownPreview->verticalScrollBar(),
                SIGNAL(valueChanged(int)),
                this, SLOT(markdownPreviewSliderValueChanged(int)));
}

MarkdownEditorWidget::~MarkdownEditorWidget()
{
    delete ui;
}

void MarkdownEditorWidget::init(NoteModel *noteModel)
{
    mNoteModel = noteModel;
    ui->markdownEditor->setText(noteModel->getNoteText());
    ui->markdownPreview->setText(noteModel->getMarkdownHtml());
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

void MarkdownEditorWidget::addTagToTagListWidget(const QString &tagName)
{
    TagCellWidget *tagCellWidget = new TagCellWidget(tagName, this);
    ui->horizontalLayout->insertWidget(ui->horizontalLayout->count() - 1, tagCellWidget);
    tagCellWidgetList.append(tagCellWidget);
}

void MarkdownEditorWidget::removeTagFromTagListWidget(const QString &tagName)
{
    if (tagName.isEmpty()) {
        if (tagCellWidgetList.length() > 0) {
            int deleteIndex = tagCellWidgetList.length() - 1;
            tagCellWidgetList[deleteIndex]->close();
            tagCellWidgetList.removeAt(deleteIndex);
        }
        return;
    }

    for (int i = 0; i < tagCellWidgetList.length(); ++i) {
        if (tagCellWidgetList[i]->getTagName() == tagName) {
            tagCellWidgetList[i]->close();
            tagCellWidgetList.removeAt(i);
            break;
        }
    }
}

bool MarkdownEditorWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->lineEdit_tag) {
        if (event->type() == QEvent::FocusIn) {
            ui->frame->setStyleSheet("QFrame#frame{border: 0.5px solid rgb(191, 191, 191);}");
            ui->pushButton_markdownPeview->setHidden(true);
            ui->pushButton_splitterPreview->setHidden(true);
        }
        else if (event->type() == QEvent::FocusOut) {
            ui->frame->setStyleSheet("");
            ui->pushButton_markdownPeview->setHidden(false);
            ui->pushButton_splitterPreview->setHidden(false);
        }
        // if lintEdit_tag is empty and press Backspace key, delete tag
        else if (event->type() == QEvent::KeyPress
                 && ((QKeyEvent *) event)->key() == Qt::Key_Backspace
                 && ui->lineEdit_tag->text().isEmpty()) {
            removeTagFromTagListWidget();
        }
    }
    return false;
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
    removeTagFromTagListWidget(tagName);
}

void MarkdownEditorWidget::on_lineEdit_tag_returnPressed()
{
    if (!ui->lineEdit_tag->text().isEmpty()) {
        addTagToTagListWidget(ui->lineEdit_tag->text());
        ui->lineEdit_tag->clear();
    }
}

void MarkdownEditorWidget::on_markdownEditor_textChanged()
{
    ui->markdownPreview->setText(mNoteModel->getMarkdownHtml());
    mNoteModel->setNoteText(ui->markdownEditor->toPlainText());
    mNoteModel->saveNoteToLocal();
}
