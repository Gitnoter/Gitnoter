#ifndef CATEGORIESWIDGET_H
#define CATEGORIESWIDGET_H

#include "categoriestablemodel.h"

#include <QWidget>
#include <QPropertyAnimation>
#include <QDesktopWidget>

namespace Ui {
class CategoriesWidget;
}

class CategoriesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CategoriesWidget(QWidget *parent = 0);
    ~CategoriesWidget() override;

signals:
    void changeCategories();

public slots:
    void resizeWindow(QSize size);

private slots:
    void animationFinished();

    void on_listWidget_data_clicked(const QModelIndex &index);
    void on_pushButton_add_clicked();
    void on_lineEdit_textChanged(const QString &arg1);

    void on_listWidget_data_doubleClicked(const QModelIndex &index);

protected:
    void mouseReleaseEvent(QMouseEvent * event) override;

private:
    Ui::CategoriesWidget *ui;
    QList<CategoriesTableModel *> m_categoriesList;
    QMultiMap<int, CategoriesTableModel *> m_categoriesSearchList;

    void setListData();
    void setSearchData(const QString &string);
};

#endif // CATEGORIESWIDGET_H
