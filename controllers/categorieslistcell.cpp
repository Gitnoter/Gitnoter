#include "categorieslistcell.h"
#include "ui_categorieslistcell.h"
#include "globals.h"

CategoriesListCell::CategoriesListCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CategoriesListCell)
{
    ui->setupUi(this);
}

CategoriesListCell::CategoriesListCell(CategoriesModel *categoriesModel) : ui(new Ui::CategoriesListCell)
{
    ui->setupUi(this);

    m_categoriesModel = categoriesModel;
    ui->lineEdit_name->setText(m_categoriesModel->getName());
    ui->label_noteCount->setText(QString::number(m_categoriesModel->getCount()));
}

CategoriesListCell::~CategoriesListCell()
{
    delete ui;
}

CategoriesModel *CategoriesListCell::getCategoriesModel() const
{
    return m_categoriesModel;
}

void CategoriesListCell::setCategoriesModel(CategoriesModel *categoriesModel)
{
    m_categoriesModel = categoriesModel;
}
