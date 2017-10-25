#include "markdowneditorwidget.h"
#include "ui_markdowneditorwidget.h"

#include <QDebug>

MarkdownEditorWidget::MarkdownEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarkdownEditorWidget)
{
    ui->setupUi(this);
}

MarkdownEditorWidget::~MarkdownEditorWidget()
{
    delete ui;
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
