#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include "tagsmodel.h"

#include <QDialog>
#include <QPropertyAnimation>
#include <QDesktopWidget>

namespace Ui
{
    class TagsWidget;
}

class TagsWidget : public QDialog
{
Q_OBJECT

public:
    explicit TagsWidget(QWidget *parent = 0);

    ~TagsWidget();

signals:

    void tagsChanged();

private slots:

    void on_pushButton_add_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_buttonBox_accepted();

private:
    Ui::TagsWidget *ui;

    void mSetListWidgetList();

    void mFiltrateListWidgetList();

};

#endif // TagsWidget_H
