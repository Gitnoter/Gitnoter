#ifndef CATEGORYLISTWIDGET_H
#define CATEGORYLISTWIDGET_H

#include "categorymodel.h"

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

signals:

    void categoriesChanged();

private slots:

    void on_pushButton_add_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_listWidget_data_doubleClicked(const QModelIndex &index);

    void on_buttonBox_accepted();

private:
    Ui::CategoryListWidget *ui;

    void mSetListWidgetList();

    void mFiltrateListWidgetList();
};

#endif // CATEGORYLISTWIDGET_H
