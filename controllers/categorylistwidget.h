#ifndef CATEGORYLISTWIDGET_H
#define CATEGORYLISTWIDGET_H

#include "notemodel.h"
#include "groupmodel.h"
#include "grouptreewidget.h"
#include "notelistwidget.h"
#include "markdowneditorwidget.h"

#include <QDialog>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QListWidgetItem>

class MarkdownEditorWidget;

namespace Ui
{
    class CategoryListWidget;
}

class CategoryListWidget : public QDialog
{
Q_OBJECT

public:
    explicit CategoryListWidget(QWidget *parent = 0);

    ~CategoryListWidget();

    void init(NoteModel *noteModel, MarkdownEditorWidget *markdownEditorWidget);
    void search();
    void append();
    void change();

private slots:
    void on_pushButton_addCategory_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_listWidget_data_doubleClicked(const QModelIndex &index);

    void on_buttonBox_accepted();

    void on_listWidget_data_itemClicked(QListWidgetItem *item);

private:
    void setCategoryList(GroupModel *groupModel);


private:
    Ui::CategoryListWidget *ui;
    MarkdownEditorWidget *mMarkdownEditorWidget;
    GroupModel *oldGroupModel;
    GroupModel *newGroupModel;

};

#endif // CATEGORYLISTWIDGET_H
