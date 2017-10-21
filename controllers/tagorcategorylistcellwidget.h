#ifndef TAGORCATEGORYLISTCELLWIDGET_H
#define TAGORCATEGORYLISTCELLWIDGET_H

#include <QWidget>

namespace Ui {
class TagOrCategoryListCellWidget;
}

class TagOrCategoryListCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TagOrCategoryListCellWidget(QString text, QString color, QWidget *parent = 0);
    ~TagOrCategoryListCellWidget();

private:
    Ui::TagOrCategoryListCellWidget *ui;
};

#endif // TAGORCATEGORYLISTCELLWIDGET_H
