#ifndef MARKDOWNEDITORWIDGET_H
#define MARKDOWNEDITORWIDGET_H

#include <QWidget>

namespace Ui {
class MarkdownEditorWidget;
}

class MarkdownEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MarkdownEditorWidget(QWidget *parent = 0);
    ~MarkdownEditorWidget();

private slots:
    void on_splitter_editor_splitterMoved(int pos, int index);

private:
    Ui::MarkdownEditorWidget *ui;

private:
    void setSplitterHandleDisable(bool b);
};

#endif // MARKDOWNEDITORWIDGET_H
