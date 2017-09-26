#ifndef CATEGORIESWIDGET_H
#define CATEGORIESWIDGET_H

#include "categoriesmodel.h"

#include <QDialog>
#include <QPropertyAnimation>
#include <QDesktopWidget>

namespace Ui
{
    class CategoriesWidget;
}

class CategoriesWidget : public QDialog
{
Q_OBJECT

public:
    explicit CategoriesWidget(QWidget *parent = 0);

    ~CategoriesWidget();

signals:

    void categoriesChanged();

private slots:

    void on_pushButton_add_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_listWidget_data_doubleClicked(const QModelIndex &index);

    void on_buttonBox_accepted();

private:
    Ui::CategoriesWidget *ui;

    void mSetListWidgetList();

    void mFiltrateListWidgetList();
};

#endif // CATEGORIESWIDGET_H
