
#include "notepreviewwidget.h"


NotePreviewWidget::NotePreviewWidget(QWidget *parent) : QTextBrowser(parent) {
}

void NotePreviewWidget::resizeEvent(QResizeEvent* event) {
    emit resize(event->size(), event->oldSize());

    // we need this, otherwise the preview is always blank
    QTextBrowser::resizeEvent(event);
}
