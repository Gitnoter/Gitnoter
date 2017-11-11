#ifndef CATEGORYLISTWIDGET_H
#define CATEGORYLISTWIDGET_H

#include "notemodel.h"
#include "groupmodel.h"

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
    explicit CategoryListWidget(QWidget *parent = 0);

    ~CategoryListWidget();

    void init(QTreeWidget *treeWidget, const QString &category);

signals:

    void categoryChanged(const QString &category);

    void categoryAppend(const QString &category);

private slots:

    void on_pushButton_addCategory_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_listWidget_data_doubleClicked(const QModelIndex &index);

    void on_buttonBox_accepted();

private:

    void setCategoryList(QList<GroupModel *> categoryModelList);

    void filtrateCategoryList();

private:
    Ui::CategoryListWidget *ui;
    QString mCategory;

};

#endif // CATEGORYLISTWIDGET_H
