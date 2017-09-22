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
    ui->widget->setGraphicsEffect(Tools::createShadowEffect());
    ui->pushButton_add->setHidden(true);

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
    emit changeCategories();
}

void CategoriesWidget::setListData(bool reread, const QString &string)
{
    if (mCategoriesModelList.length() == 0 || reread) {
        mCategoriesModelList = gDatabase->selectCategories();
    }

    if (!string.isEmpty()) {
        mCategoriesModelSearchList.clear();
        for (int i = 0; i < mCategoriesModelList.length(); ++i) {
            int searchIndex = mCategoriesModelList[i]->getName().indexOf(string, 0, Qt::CaseInsensitive);
            if (searchIndex != -1) {
                mCategoriesModelSearchList.append(mCategoriesModelList[i]);
            }
        }
    }
    else {
        mCategoriesModelSearchList = mCategoriesModelList;
    }

    ui->listWidget_data->clear();
    for (int i = 0; i < mCategoriesModelSearchList.length(); ++i) {
        ui->listWidget_data->addItem(mCategoriesModelSearchList[i]->getName());

        if (gOpenNoteModel->categoriesModel->getName() == mCategoriesModelSearchList[i]->getName()) {
            ui->listWidget_data->setItemSelected(ui->listWidget_data->item(i), true);
        }
    }
}

void CategoriesWidget::on_listWidget_data_clicked(const QModelIndex &index)
{
    if (ui->lineEdit->displayText().isEmpty()) {
        gOpenNoteModel->categoriesModel = mCategoriesModelList[index.row()];
    }
    else {
        int i = 0;
        for (auto &&item : mCategoriesModelSearchList) {
            if (i == index.row()) {
                gOpenNoteModel->categoriesModel = item;
            }
            i += 1;
        }
    }
}

void CategoriesWidget::on_listWidget_data_doubleClicked(const QModelIndex &index)
{
    if (ui->lineEdit->displayText().isEmpty()) {
        gOpenNoteModel->categoriesModel = mCategoriesModelList[index.row()];
    }
    else {
        int i = 0;
        for (auto &&item : mCategoriesModelSearchList) {
            if (i == index.row()) {
                gOpenNoteModel->categoriesModel = item;
            }
            i += 1;
        }
    }
    this->animationFinished();
}

void CategoriesWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "geometry");
    pAnimation->setDuration(250);
    pAnimation->setEndValue(QRect(0, -height(), width(), height()));
    pAnimation->setStartValue(QRect(0, 0, width(), height()));
    pAnimation->setEasingCurve(QEasingCurve::Linear);
    pAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    connect(pAnimation, &QPropertyAnimation::finished, this, &CategoriesWidget::animationFinished);
}

void CategoriesWidget::on_pushButton_add_clicked()
{
    if (!ui->lineEdit->displayText().isEmpty()) {
        gDatabase->insertCategories(ui->lineEdit->displayText());
        mCategoriesModelList = gDatabase->selectCategories();
        ui->lineEdit->clear();
    }
}

void CategoriesWidget::on_lineEdit_textChanged(const QString &arg1)
{
    arg1.isEmpty() ? setListData() : setListData(false, arg1);
    ui->pushButton_add->setHidden(ui->listWidget_data->count() != 0);
}
