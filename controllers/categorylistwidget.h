#ifndef CATEGORYLISTWIDGET_H
#define CATEGORYLISTWIDGET_H

#include "categorymodellist.h"
#include "notemodellist.h"

#include <QDialog>
#include <QPropertyAnimation>
#include <QDesktopWidget>

namespace Ui
{
    class CategoryListWidget;
}

class CategoryListWidget : public QDialog
{
Q_OBJECT

public:
    explicit CategoryListWidget(QWidget *parent = 0, NoteModel *noteModel = nullptr);

    ~CategoryListWidget();

signals:

    void categoriesChanged();

private slots:

    void on_pushButton_add_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_listWidget_data_doubleClicked(const QModelIndex &index);

    void on_buttonBox_accepted();

private:

    void mSetListWidgetList();

    void mFiltrateListWidgetList();

private:
    Ui::CategoryListWidget *ui;
    NoteModel *mNoteModel;

};

#endif // CATEGORYLISTWIDGET_H
