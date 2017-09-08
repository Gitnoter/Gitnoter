#include "categorieslistcell.h"
#include "ui_categorieslistcell.h"
#include "tools/globals.h"

CategoriesListCell::CategoriesListCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CategoriesListCell)
{
    ui->setupUi(this);
}

CategoriesListCell::CategoriesListCell(CategoriesTableModel *categoriesTableModel) : ui(new Ui::CategoriesListCell)
{
    ui->setupUi(this);

    m_categoriesTableModel = categoriesTableModel;
    ui->lineEdit_name->setText(m_categoriesTableModel->getName());
    ui->label_noteCount->setText(QString::number(m_categoriesTableModel->getCount()));
}

CategoriesListCell::~CategoriesListCell()
{
    delete ui;
}

CategoriesTableModel *CategoriesListCell::getCategoriesTableModel() const
{
    return m_categoriesTableModel;
}

void CategoriesListCell::setCategoriesTableModel(CategoriesTableModel *categoriesTableModel)
{
    m_categoriesTableModel = categoriesTableModel;
}
