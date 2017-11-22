#ifndef MENUBAR_H
#define MENUBAR_H

#include <QMenuBar>

class MenuBar : public QMenuBar
{
    Q_OBJECT
public:
    explicit MenuBar(QWidget *parent = 0);

private:
    void setupUI();
};

#endif // MENUBAR_H
