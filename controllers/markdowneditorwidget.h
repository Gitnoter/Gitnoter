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
#include <QMenuBar>
#include <QPrinter>

namespace Ui
{
    class MarkdownEditorWidget;
}

class CategoryListWidget;
class MainWindow;
class GroupTreeWidget;

class MarkdownEditorWidget : public QWidget
{
Q_OBJECT

public:
    explicit MarkdownEditorWidget(QWidget *parent = 0);

    ~MarkdownEditorWidget();

    void init(const QString &uuid, MainWindow *mainWindow = 0);
    void init(NoteModel *noteModel, MainWindow *mainWindow = 0);

public slots:
    void appendTag(const QString &tag);
    void removeTag(QString tagName = "");

    void changeCategory(const QString &category);
    void appendCategory(const QString &category);

    void modifyNote();
    void saveNote();

    void openPath();
    void print(QPrinter *printer);
    void copyLine();
    void removeLine();
    void removeSelectedText();
    void clearText();
    void pasteHtml();
    void webSearchText();

    void showSearchFindWidget();
    void showSearchReplaceWidget();

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

    void on_pushButton_splitterPreview_clicked();

    void on_pushButton_markdownPeview_clicked();

    void on_markdownEditor_customContextMenuRequested(const QPoint &pos);

private:
    void setupUi();

    bool eventFilter(QObject *object, QEvent *event);

    void setTagList();  // save tagList to noteModel

    void setOpenNote();

    void setSplitterSizes();

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
