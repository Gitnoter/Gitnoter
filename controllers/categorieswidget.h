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

public slots:
    void resizeWindow(QSize size);

private slots:
    void on_pushButton_clicked();

private:
    Ui::CategoriesWidget *ui;

};

#endif // CATEGORIESWIDGET_H
