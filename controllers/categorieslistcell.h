#ifndef CATEGORIESLISTCELL_H
#define CATEGORIESLISTCELL_H

#include "categoriestablemodel.h"

#include <QWidget>

namespace Ui {
class CategoriesListCell;
}

class CategoriesListCell : public QWidget
{
    Q_OBJECT

public:
    explicit CategoriesListCell(QWidget *parent = 0);
    CategoriesListCell(CategoriesTableModel *categoriesTableModel);
    ~CategoriesListCell();

private:
    Ui::CategoriesListCell *ui;
    CategoriesTableModel *m_categoriesTableModel;
public:
    CategoriesTableModel *getCategoriesTableModel() const;

    void setCategoriesTableModel(CategoriesTableModel *categoriesTableModel);

private slots:
    void on_pushButton_clicked();
};

#endif // CATEGORIESLISTCELL_H
