#include "categorieswidget.h"
#include "mainwindow.h"
#include "ui_categorieswidget.h"
#include "tools.h"

#include <QGraphicsDropShadowEffect>
#include <QDebug>

CategoriesWidget::CategoriesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CategoriesWidget)
{
    ui->setupUi(this);
    ui->listWidget_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->widget_2->setGraphicsEffect(Tools::createShadowEffect());

    auto *mainWindow = (MainWindow *) parentWidget();
    resizeWindow(mainWindow->geometry().size());

    QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "geometry");
    pAnimation->setDuration(250);
    pAnimation->setStartValue(QRect(0, -height(), width(), height()));
    pAnimation->setEndValue(QRect(0, 0, width(), height()));
    pAnimation->setEasingCurve(QEasingCurve::Linear);
    pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

CategoriesWidget::~CategoriesWidget()
{
    delete ui;
}

void CategoriesWidget::resizeWindow(QSize size)
{
    setGeometry(x(), y(), size.width(), size.height());
}

void CategoriesWidget::on_pushButton_cancel_clicked()
{
    QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "geometry");
    pAnimation->setDuration(250);
    pAnimation->setEndValue(QRect(0, -height(), width(), height()));
    pAnimation->setStartValue(QRect(0, 0, width(), height()));
    pAnimation->setEasingCurve(QEasingCurve::Linear);
    pAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    connect(pAnimation, &QPropertyAnimation::finished, this, &CategoriesWidget::animationFinished);
}

void CategoriesWidget::animationFinished()
{
    this->close();
}
