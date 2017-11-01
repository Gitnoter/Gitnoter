#ifndef MARKDOWNEDITORWIDGET_H
#define MARKDOWNEDITORWIDGET_H

#include "tagcellwidget.h"
#include "notemodellist.h"

#include <QWidget>
#include <QPushButton>

namespace Ui {
class MarkdownEditorWidget;
}

class MarkdownEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MarkdownEditorWidget(QWidget *parent = 0);
    ~MarkdownEditorWidget();

    void init(NoteModel *noteModel);

private slots:
    void on_splitter_editor_splitterMoved(int pos, int index);

    /**
     * Moves the note view scrollbar when the note edit scrollbar was moved
     */
    void markdownEditorSliderValueChanged(int value, bool force = false);

    /**
     * Moves the note edit scrollbar when the note view scrollbar was moved
     */
    void markdownPreviewSliderValueChanged(int value, bool force = false);

    void onTagCellWidgetClicked(const QString tagName);

    void on_lineEdit_tag_returnPressed();

    void on_markdownEditor_textChanged();

private:
    Ui::MarkdownEditorWidget *ui;
    NoteModel *mNoteModel;
    QList<TagCellWidget *> tagCellWidgetList;

private:
    void setSplitterHandleDisable(bool b);

    void addTagToTagListWidget(const QString &tagName);

    void removeTagFromTagListWidget(const QString &tagName = "");

    bool eventFilter(QObject* object, QEvent* event);

};

#endif // MARKDOWNEDITORWIDGET_H
