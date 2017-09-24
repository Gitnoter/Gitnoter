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

    this->mSetListWidgetList();
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
    emit categoriesChanged();
}

void CategoriesWidget::mSetListWidgetList()
{
    ui->listWidget_data->clear();
    for (int i = 0; i < mCategoriesModelList.length(); ++i) {
        ui->listWidget_data->addItem(mCategoriesModelList[i]->getName());
        ui->listWidget_data->item(i)->setData(Qt::UserRole, QVariant::fromValue(mCategoriesModelList[i]));
        if (Global::openNoteModel->categoriesModel->getName() == mCategoriesModelList[i]->getName()) {
            ui->listWidget_data->setItemSelected(ui->listWidget_data->item(i), true);
        }
    }
}

void CategoriesWidget::mFiltrateListWidgetList()
{
    int showCount = 0;
    QString text = ui->lineEdit->displayText();
    if (!text.isEmpty()) {
        mCategoriesModelList.clear();
        for (int i = 0; i < mCategoriesModelList.length(); ++i) {
            int index = mCategoriesModelList[i]->getName().indexOf(text, 0, Qt::CaseInsensitive);
            QListWidgetItem *listWidgetItem = ui->listWidget_data->item(i);
            bool isHidden = text.isEmpty() ? false : index == -1;
            listWidgetItem->setHidden(isHidden);
            if (!isHidden) showCount += 1;
        }
    }

    ui->pushButton_add->setHidden(showCount != 0);
}

void CategoriesWidget::on_listWidget_data_clicked(const QModelIndex &index)
{
    if (ui->lineEdit->displayText().isEmpty()) {
        Global::openNoteModel->categoriesModel = mCategoriesModelList[index.row()];
    }
    else {
        int i = 0;
        for (auto &&item : mCategoriesModelList) {
            if (i == index.row()) {
                Global::openNoteModel->categoriesModel = item;
            }
            i += 1;
        }
    }
}

void CategoriesWidget::on_listWidget_data_doubleClicked(const QModelIndex &index)
{
    if (ui->lineEdit->displayText().isEmpty()) {
        Global::openNoteModel->categoriesModel = mCategoriesModelList[index.row()];
    }
    else {
        int i = 0;
        for (auto &&item : mCategoriesModelList) {
            if (i == index.row()) {
                Global::openNoteModel->categoriesModel = item;
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
//        gDatabase->insertCategories(ui->lineEdit->displayText());
//        mCategoriesModelList = gDatabase->selectCategories();
//        ui->lineEdit->clear();
    }
}

void CategoriesWidget::on_lineEdit_textChanged(const QString &arg1)
{
    mFiltrateListWidgetList();
}
