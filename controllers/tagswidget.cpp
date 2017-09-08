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

    QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "geometry");
    pAnimation->setDuration(250);
    pAnimation->setStartValue(QRect(0, -height(), width(), height()));
    pAnimation->setEndValue(QRect(0, 0, width(), height()));
    pAnimation->setEasingCurve(QEasingCurve::Linear);
    pAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    this->setListData(true);
}

TagsWidget::~TagsWidget()
{
    delete ui;
}

void TagsWidget::resizeWindow(QSize size)
{
    setGeometry(x(), y(), size.width(), size.height());
}

void TagsWidget::animationFinished()
{
    auto selectedIndexes = ui->listWidget_data->selectionModel()->selectedIndexes();
    for (auto &&index : selectedIndexes) {

    }
    this->close();
    emit changeCategories();
}

void TagsWidget::setListData(bool reread, const QString &string)
{
    if (!string.isEmpty()) {
        m_tagTableModelList.clear();
        for (int i = 0; i < m_tagTableModelList.length(); ++i) {
            int searchIndex = m_tagTableModelList[i]->getName().indexOf(string);
            if (searchIndex != -1) {
                m_tagTableModelSearchList.append(m_tagTableModelList[i]);
            }
        }
    }
    else {
        if (m_tagTableModelList.length() == 0 || reread) {
            m_tagTableModelList = g_database->selectTagsTable();
        }
        m_tagTableModelSearchList = m_tagTableModelList;
    }

    ui->listWidget_data->clear();
    for (int i = 0; i < m_tagTableModelList.length(); ++i) {
        ui->listWidget_data->addItem(m_tagTableModelList[i]->getName());

        if (g_noteModel->categoriesTableModel->getName() == m_tagTableModelList[i]->getName()) {
            ui->listWidget_data->setItemSelected(ui->listWidget_data->item(i), true);
        }
    }
}

void TagsWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "geometry");
    pAnimation->setDuration(250);
    pAnimation->setEndValue(QRect(0, -height(), width(), height()));
    pAnimation->setStartValue(QRect(0, 0, width(), height()));
    pAnimation->setEasingCurve(QEasingCurve::Linear);
    pAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    connect(pAnimation, &QPropertyAnimation::finished, this, &TagsWidget::animationFinished);
}
void TagsWidget::on_pushButton_add_clicked()
{
    if (!ui->lineEdit->displayText().isEmpty()) {
        g_database->insertTagsTable(ui->lineEdit->displayText());
        setListData(true);
        ui->lineEdit->clear();
    }
}

void TagsWidget::on_lineEdit_textChanged(const QString &arg1)
{
    setListData(false, arg1);
    ui->pushButton_add->setHidden(ui->listWidget_data->count() != 0);
}
