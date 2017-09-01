#include "categorieswidget.h"
#include "mainwindow.h"
#include "ui_categorieswidget.h"
#include "tools.h"
#include "globals.h"

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

    this->setListData();
}

CategoriesWidget::~CategoriesWidget()
{
    delete ui;
}

void CategoriesWidget::resizeWindow(QSize size)
{
    setGeometry(x(), y(), size.width(), size.height());
}

void CategoriesWidget::animationFinished()
{
    this->close();
}

void CategoriesWidget::setListData()
{
    tagsList = g_database->selectCategoriesTable();
    for (int i = 0; i < tagsList.length(); ++i) {
        ui->listWidget_data->addItem(tagsList[i]->getName());

        if (g_noteModel->categoriesTableModel->getName() == tagsList[i]->getName()) {
            ui->listWidget_data->setItemSelected(ui->listWidget_data->item(i), true);
        }
    }
}

void CategoriesWidget::on_listWidget_data_clicked(const QModelIndex &index)
{
    g_noteModel->categoriesTableModel = tagsList[index.row()];
}

void CategoriesWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    emit changeCategories();

    QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "geometry");
    pAnimation->setDuration(250);
    pAnimation->setEndValue(QRect(0, -height(), width(), height()));
    pAnimation->setStartValue(QRect(0, 0, width(), height()));
    pAnimation->setEasingCurve(QEasingCurve::Linear);
    pAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    connect(pAnimation, &QPropertyAnimation::finished, this, &CategoriesWidget::animationFinished);
}