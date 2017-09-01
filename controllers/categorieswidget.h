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

protected:
    void mouseReleaseEvent(QMouseEvent * event) override;

private:
    Ui::CategoriesWidget *ui;
    QList<CategoriseTableModel *> tagsList;

    void setListData();

};

#endif // CATEGORIESWIDGET_H
