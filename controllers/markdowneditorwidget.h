#ifndef MARKDOWNEDITORWIDGET_H
#define MARKDOWNEDITORWIDGET_H

#include "tagcellwidget.h"
#include "notemodel.h"
#include "categorylistwidget.h"

#include <QWidget>
#include <QPushButton>

namespace Ui
{
    class MarkdownEditorWidget;
}

class MarkdownEditorWidget : public QWidget
{
Q_OBJECT

public:
    explicit MarkdownEditorWidget(QWidget *parent = 0);

    ~MarkdownEditorWidget();

    void init(NoteModel *noteModel, QTreeWidget *treeWidget, QListWidget *listWidget);

signals:

    void noteModelChanged(NoteModel *noteModel);

    void tagAppend(const QString &tag);

    void tagDeleted(const QString &tag);

    void categoryAppend(const QString &tag);

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

    void onCategoryChanged(const QString &category);

    void onCategoryAppend(const QString &category);

    void onTagCellWidgetClicked(const QString tagName);

    void on_lineEdit_tag_returnPressed();

    void on_markdownEditor_textChanged();

    void on_pushButton_category_clicked();

private:
    Ui::MarkdownEditorWidget *ui;
    NoteModel *mNoteModel;
    QTreeWidget *mTreeWidget;
    QListWidget *mListWidget;
    CategoryListWidget *categoryListWidget;

private:
    void setSplitterHandleDisable(bool b);

    void addTag(const QString &tagName);

    void removeTag(const QString &tagName = "");

    void setTagList();

    bool eventFilter(QObject *object, QEvent *event);

};

#endif // MARKDOWNEDITORWIDGET_H
