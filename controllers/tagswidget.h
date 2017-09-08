#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include "tagtablemodel.h"

#include <QWidget>
#include <QPropertyAnimation>
#include <QDesktopWidget>

namespace Ui {
class TagsWidget;
}

class TagsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TagsWidget(QWidget *parent = 0);
    ~TagsWidget() override;

signals:
    void changeCategories();

public slots:
    void resizeWindow(QSize size);

private slots:
    void animationFinished();

    void on_pushButton_add_clicked();
    void on_lineEdit_textChanged(const QString &arg1);

protected:
    void mouseReleaseEvent(QMouseEvent * event) override;

private:
    Ui::TagsWidget *ui;
    QList<TagTableModel *> m_tagTableModelList;
    QList<TagTableModel *> m_tagTableModelSearchList;

    void setListData(bool reread = true, const QString &string = "");


};

#endif // TagsWidget_H
