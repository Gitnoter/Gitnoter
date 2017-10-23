#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "document.h"
#include "categorieswidget.h"
#include "notemodel.h"
#include "configmodel.h"
#include "tagswidget.h"
#include "settingdialog.h"
#include "aboutdialog.h"

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <QTimer>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

signals:
    void noteListItemClicked(QListWidgetItem *item);

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_splitter_splitterMoved(int pos, int index);

private:
    Ui::MainWindow *ui;

private:
    void setupUi();
    void setNoteList();
};

#endif // MAINWINDOW_H
