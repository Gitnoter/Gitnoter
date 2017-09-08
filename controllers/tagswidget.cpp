#include "tagswidget.h"
#include "mainwindow.h"
#include "ui_tagswidget.h"
#include "tools/tools.h"
#include "tools/globals.h"

#include <QDebug>

TagsWidget::TagsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagsWidget)
{
    ui->setupUi(this);
    ui->listWidget_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->widget_2->setGraphicsEffect(Tools::createShadowEffect());
    ui->pushButton_add->setHidden(true);

    auto *mainWindow = (MainWindow *) parentWidget();
    resizeWindow(mainWindow->geometry().size());

    QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "geometry");
    pAnimation->setDuration(250);
    pAnimation->setStartValue(QRect(0, -height(), width(), height()));
    pAnimation->setEndValue(QRect(0, 0, width(), height()));
    pAnimation->setEasingCurve(QEasingCurve::Linear);
    pAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    m_categoriesList = g_database->selectCategoriesTable();
    this->setListData();
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
    this->close();
    emit changeCategories();
}

void TagsWidget::setListData(bool reread, const QString &string)
{
    if (!string.isEmpty()) {
        m_categoriesSearchList.clear();
        for (int i = 0; i < m_categoriesList.length(); ++i) {
            int searchIndex = m_categoriesList[i]->getName().indexOf(string);
            if (searchIndex != -1) {
                m_categoriesSearchList.append(m_categoriesList[i]);
            }
        }
    }
    else {
        if (reread) {
            m_categoriesList = g_database->selectCategoriesTable();
        }
        m_categoriesSearchList = m_categoriesList;
    }

    ui->listWidget_data->clear();
    for (int i = 0; i < m_categoriesSearchList.length(); ++i) {
        ui->listWidget_data->addItem(m_categoriesSearchList[i]->getName());

        if (g_noteModel->categoriesTableModel->getName() == m_categoriesSearchList[i]->getName()) {
            ui->listWidget_data->setItemSelected(ui->listWidget_data->item(i), true);
        }
    }
}

void TagsWidget::on_listWidget_data_clicked(const QModelIndex &index)
{
    if (ui->lineEdit->displayText().isEmpty()) {
        g_noteModel->categoriesTableModel = m_categoriesList[index.row()];
    }
    else {
        int i = 0;
        for (auto &&item : m_categoriesSearchList) {
            if (i == index.row()) {
                g_noteModel->categoriesTableModel = item;
            }
            i += 1;
        }
    }
}

void TagsWidget::on_listWidget_data_doubleClicked(const QModelIndex &index)
{
    if (ui->lineEdit->displayText().isEmpty()) {
        g_noteModel->categoriesTableModel = m_categoriesList[index.row()];
    }
    else {
        int i = 0;
        for (auto &&item : m_categoriesSearchList) {
            if (i == index.row()) {
                g_noteModel->categoriesTableModel = item;
            }
            i += 1;
        }
    }
    this->animationFinished();
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
        g_database->insertCategoriesTable(ui->lineEdit->displayText());
        m_categoriesList = g_database->selectCategoriesTable();
        ui->lineEdit->clear();
    }
}

void TagsWidget::on_lineEdit_textChanged(const QString &arg1)
{
    arg1.isEmpty() ? setListData() : setListData(false, arg1);
    ui->pushButton_add->setHidden(ui->listWidget_data->count() != 0);
}
