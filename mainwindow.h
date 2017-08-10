#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "document.h"

#include <QMainWindow>
#include <QString>
#include <QMap>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QMap<QString, QPushButton *> menuPushButtons;

private:
    Ui::MainWindow *ui;
    Document m_content;

    void textChangedAndUpdatePreview();
    void menuPushButtonClicked();
};

#endif // MAINWINDOW_H
