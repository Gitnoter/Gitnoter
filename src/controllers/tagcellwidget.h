#ifndef TAGCELLWIDGET_H
#define TAGCELLWIDGET_H

#include <QWidget>

namespace Ui {
class TagCellWidget;
}

class TagCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TagCellWidget(const QString tagName, QWidget *parent = 0);
    ~TagCellWidget();

    QString getTagName() const;

signals:
    void clicked(const QString tagName);

private slots:
    void on_pushButton_tag_clicked();

    void on_pushButton_delete_clicked();

private:
    Ui::TagCellWidget *ui;

    QString tagName;
};

#endif // TAGCELLWIDGET_H
