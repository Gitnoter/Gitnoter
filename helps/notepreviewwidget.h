#include <QTextBrowser>
#include <QResizeEvent>

class NotePreviewWidget : public QTextBrowser
{
    Q_OBJECT

public:
    explicit NotePreviewWidget(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent* event);

signals:
    void resize(QSize size, QSize oldSize = QSize());

private:
};
