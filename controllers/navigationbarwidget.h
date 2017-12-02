#ifndef NAVIGATIONBARWIDGET_H
#define NAVIGATIONBARWIDGET_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTextDocument>

class NavigationBarWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit NavigationBarWidget(QWidget *parent = 0);
    void init(QTextDocument *document);

signals:
    void positionClicked(int position);

private slots:
    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    QMap<int, QTreeWidgetItem *> mLastHeadingItemList;

    QTreeWidgetItem *findSuitableParentItem(int elementType);
};

#endif // NAVIGATIONBARWIDGET_H