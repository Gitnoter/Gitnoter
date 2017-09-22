#ifndef CATEGORIESLISTCELL_H
#define CATEGORIESLISTCELL_H

#include "categoriesmodel.h"

#include <QWidget>

namespace Ui {
class CategoriesListCell;
}

class CategoriesListCell : public QWidget
{
    Q_OBJECT

public:
    explicit CategoriesListCell(QWidget *parent = 0);
    CategoriesListCell(CategoriesModel *categoriesModel);
    ~CategoriesListCell();

private:
    Ui::CategoriesListCell *ui;
    CategoriesModel *m_categoriesModel;
public:
    CategoriesModel *getCategoriesModel() const;

    void setCategoriesModel(CategoriesModel *categoriesModel);

};

#endif // CATEGORIESLISTCELL_H
