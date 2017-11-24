#ifndef MENUBAR_H
#define MENUBAR_H

#include <QMainWindow>

namespace Ui {
class MenuBar;
}

class MenuBar : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuBar(QWidget *parent = 0);
    ~MenuBar();

private:
    void setupUi(QWidget *parent);

private:
    Ui::MenuBar *ui;

};

#endif // MENUBAR_H
