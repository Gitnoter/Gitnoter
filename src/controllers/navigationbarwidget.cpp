#include "navigationbarwidget.h"

#include <qmarkdowntextedit/markdownhighlighter.h>

NavigationBarWidget::NavigationBarWidget(QWidget *parent)
    : QTreeWidget(parent) {
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
}

/**
 * Emits the positionClicked signal to jump to the clicked navigation item's
 * position
 */
void NavigationBarWidget::onCurrentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    if (current == NULL) {
        return;
    }

    emit positionClicked(current->data(0, Qt::UserRole).toInt());
}

/**
 * Parses a text document and builds the navigation tree for it
 */
void NavigationBarWidget::init(QTextDocument *document) {
    const QSignalBlocker blocker(this);
    Q_UNUSED(blocker);

    clear();
    mLastHeadingItemList.clear();

    for (int i = 0; i < document->blockCount(); i++) {
        QTextBlock block = document->findBlockByNumber(i);
        int elementType = block.userState();
        QString text = block.text();

        // ignore all non headline types
        if ((elementType < MarkdownHighlighter::H1) ||
                (elementType > MarkdownHighlighter::H6)) {
            continue;
        }

        text.remove(QRegularExpression("^#+"))
                .remove(QRegularExpression("#+$"))
                .remove(QRegularExpression("^\\s+"))
                .remove(QRegularExpression("^=+$"))
                .remove(QRegularExpression("^-+$"));

        if (text.isEmpty()) {
            continue;
        }

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, text);
        item->setData(0, Qt::UserRole, block.position());
        item->setToolTip(0, tr("headline %1").arg(
                elementType - MarkdownHighlighter::H1 + 1));

        // attempt to find a suitable parent item for the element type
        QTreeWidgetItem *lastHigherItem = findSuitableParentItem(elementType);

        if (lastHigherItem == NULL) {
            // if there wasn't a last higher level item then add the current
            // item to the top level
            addTopLevelItem(item);
        } else {
            // if there was a last higher level item then add the current
            // item as child of that item
            lastHigherItem->addChild(item);
        }

        mLastHeadingItemList[elementType] = item;
    }

    expandAll();
}

/**
 * Attempts to find a suitable parent item for the element type
 */
QTreeWidgetItem * NavigationBarWidget::findSuitableParentItem(int elementType) {
    elementType--;
    QTreeWidgetItem *lastHigherItem = mLastHeadingItemList[elementType];

    return ((lastHigherItem == NULL) &&
            (elementType > MarkdownHighlighter::H1)) ?
           findSuitableParentItem(elementType) : lastHigherItem;
}
