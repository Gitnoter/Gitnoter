#include "tagswidget.h"
#include "mainwindow.h"
#include "ui_tagswidget.h"
#include "tools.h"
#include "globals.h"

#include <QDebug>

TagsWidget::TagsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagsWidget)
{
    ui->setupUi(this);
    ui->listWidget_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->widget->setGraphicsEffect(Tools::createShadowEffect());
    ui->pushButton_add->setHidden(true);

    auto *mainWindow = (MainWindow *) parentWidget();
    resizeWindow(mainWindow->geometry().size());

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(250);
    animation->setStartValue(QRect(0, -height(), width(), height()));
    animation->setEndValue(QRect(0, 0, width(), height()));
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    this->setListData(true);
    ui->listWidget_data->setFocus();
}

TagsWidget::~TagsWidget()
{
    delete ui;
}

void TagsWidget::resizeWindow(QSize size)
{
    setGeometry(x(), y(), size.width(), size.height());
}

void TagsWidget::onAnimationFinished()
{
    auto selectedIndexes = ui->listWidget_data->selectionModel()->selectedIndexes();
    gOpenNoteModel->tagsModelList->clear();
    for (auto &&index : selectedIndexes) {
        gOpenNoteModel->tagsModelList->append(mTagsModelList[index.row()]);
    }
    this->close();
    emit changeTags();
}

void TagsWidget::setListData(bool reread, const QString &string)
{
    if (mTagsModelList.length() == 0 || reread) {
        mTagsModelList = gDatabase->selectTags();
    }

    if (!string.isEmpty()) {
        mtagsModelSearchList.clear();
        for (int i = 0; i < mTagsModelList.length(); ++i) {
            int searchIndex = mTagsModelList[i]->getName().indexOf(string, 0, Qt::CaseInsensitive);
            if (searchIndex != -1) {
                mtagsModelSearchList.append(mTagsModelList[i]);
            }
        }
    }
    else {
        mtagsModelSearchList = mTagsModelList;
    }

    ui->listWidget_data->clear();
    for (int i = 0; i < mtagsModelSearchList.length(); ++i) {
        ui->listWidget_data->addItem(mtagsModelSearchList[i]->getName());

        for (auto &&item : *gOpenNoteModel->tagsModelList) {
            if (item->getName() == mtagsModelSearchList[i]->getName()) {
                ui->listWidget_data->setItemSelected(ui->listWidget_data->item(i), true);
            }
        }
    }
}

void TagsWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(250);
    animation->setEndValue(QRect(0, -height(), width(), height()));
    animation->setStartValue(QRect(0, 0, width(), height()));
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    connect(animation, &QPropertyAnimation::finished, this, &TagsWidget::onAnimationFinished);
}

void TagsWidget::on_pushButton_add_clicked()
{
    if (!ui->lineEdit->displayText().isEmpty()) {
        gDatabase->insertTags(ui->lineEdit->displayText());
        setListData(true);
        ui->lineEdit->clear();
    }
}

void TagsWidget::on_lineEdit_textChanged(const QString &arg1)
{
    setListData(false, arg1);
    ui->pushButton_add->setHidden(ui->listWidget_data->count() != 0);
}
