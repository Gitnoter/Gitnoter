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
    ~CategoriesWidget() override;

public slots:
    void resizeWindow(QSize size);

private slots:
    void on_pushButton_cancel_clicked();

    void animationFinished();

private:
    Ui::CategoriesWidget *ui;

};

#endif // CATEGORIESWIDGET_H
