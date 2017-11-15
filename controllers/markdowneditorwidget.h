#ifndef MARKDOWNEDITORWIDGET_H
#define MARKDOWNEDITORWIDGET_H

#include "tagcellwidget.h"
#include "notemodel.h"
#include "categorylistwidget.h"
#include "grouptreewidget.h"
#include "notelistwidget.h"
#include "mainwindow.h"

#include <QWidget>
#include <QPushButton>

class CategoryListWidget;
class MainWindow;

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

    void init(const QString &uuid, MainWindow *mainWindow = 0);
    void init(NoteModel *noteModel, MainWindow *mainWindow = 0);

    void appendTag();
    void removeTag(const QString &tagName = "");

    void changeCategory(const QString &category);
    void appendCategory(const QString &category);

    void modifyNote();

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

    void on_pushButton_category_clicked();

private:
    void setSplitterHandleDisable(bool b);

    bool eventFilter(QObject *object, QEvent *event);

    void setTagList();  // save tagList to noteModel

    void setOpenNote();

public:
    MainWindow *mainWindow() { return mMainWindow; }

    NoteModel *noteModel() {return mNoteModel; }

private:
    Ui::MarkdownEditorWidget *ui;
    MainWindow *mMainWindow;
    CategoryListWidget *mCategoryListWidget;

    NoteModel *mNoteModel;
    QList<TagCellWidget *> mTagCellWidgetList;

};

#endif // MARKDOWNEDITORWIDGET_H
