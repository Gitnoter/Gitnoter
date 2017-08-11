#ifndef CATEGORIESWIDGET_H
#define CATEGORIESWIDGET_H

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
    ~CategoriesWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::CategoriesWidget *ui;
    QPropertyAnimation *pAnimation;
    QDesktopWidget *pDesktopWidget;
};

#endif // CATEGORIESWIDGET_H
