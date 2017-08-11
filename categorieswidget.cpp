#include "categorieswidget.h"
#include "ui_categorieswidget.h"

CategoriesWidget::CategoriesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CategoriesWidget)
{
    ui->setupUi(this);

//    this->pAnimation = new QPropertyAnimation(this, "geometry");
//    this->pDesktopWidget = QApplication::desktop();
//    int x = (this->pDesktopWidget->availableGeometry().width() - width()) / 2 - this->width() / 2;

//    this->pAnimation->setDuration(300);
//    this->pAnimation->setStartValue(QRect(x, -this->height(), this-width(), this->height()));
//    this->pAnimation->setEndValue(QRect(x, -12, this->width(), this->height()));
//    this->pAnimation->setEasingCurve(QEasingCurve::Linear);
//    this->pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

CategoriesWidget::~CategoriesWidget()
{
    delete ui;
}

void CategoriesWidget::on_pushButton_clicked()
{
    int x = (this->pDesktopWidget->availableGeometry().width() - width()) / 2 - this->width() / 2;

    this->pAnimation->setDuration(300);
    this->pAnimation->setEndValue(QRect(x, -this->height(), this->width(), this->height()));
    this->pAnimation->setEasingCurve(QEasingCurve::Linear);
    this->pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}
