#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include "categoriestablemodel.h"

#include <QWidget>
#include <QPropertyAnimation>
#include <QDesktopWidget>

namespace Ui {
class TagsWidget;
}

class TagsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TagsWidget(QWidget *parent = 0);
    ~TagsWidget() override;

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
    Ui::TagsWidget *ui;
    QList<CategoriesTableModel *> m_categoriesList;
    QList<CategoriesTableModel *> m_categoriesSearchList;

    void setListData(bool reread = true, const QString &string = "");
};

#endif // TagsWidget_H
