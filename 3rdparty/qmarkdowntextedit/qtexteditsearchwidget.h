/*
 * Copyright (c) 2014-2017 Patrizio Bekerle -- http://www.bekerle.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#pragma once

#include <QWidget>
#include <QPlainTextEdit>
#include <QMenu>

class PopupMenu : public QMenu
{
Q_OBJECT
public:
    enum SearchMode {
        PlainTextMode,
        WholeWordsMode,
        RegularExpressionMode
    };

    explicit PopupMenu(QWidget *parent = 0, PopupMenu::SearchMode mode = PopupMenu::PlainTextMode, bool matchCaseSensitive = false);

    void setSearchMode(SearchMode mode);
    SearchMode searchModel() { return mSearchMode; }
    bool matchCaseSensitiveChecked();

signals:
    void actionTriggered(PopupMenu::SearchMode searchMode, bool matchCaseSensitive = false);

private slots:
    void onActionPlainTextTriggered(bool triggered);
    void onActionWholeWordsTriggered(bool triggered);
    void onActionRegularExpressionTriggered(bool triggered);
    void onActionMatchCaseSensitiveTriggered(bool triggered);

private:
    void setupUi(QWidget *parent = 0);
    void seActionChecked(QAction *action);

private:
    QAction *action_plainText;
    QAction *action_wholeWords;
    QAction *action_regularExpression;
    QAction *action_matchCaseSensitive;

    SearchMode mSearchMode;

};

namespace Ui {
class QTextEditSearchWidget;
}

class QTextEditSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QTextEditSearchWidget(QPlainTextEdit *parent = 0);
    bool doSearch(bool searchDown = true, bool allowRestartAtTop = true);
    bool isReplace();
    PopupMenu *popupMenu() { return mPopupMenu; }
    ~QTextEditSearchWidget();

private:
    Ui::QTextEditSearchWidget *ui;
    PopupMenu *mPopupMenu;

protected:
    QPlainTextEdit *_textEdit;
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void activate(bool replace = false);
    void deactivate();
    void doSearchDown();
    void doSearchUp();
    void setReplaceMode(bool enabled);
    void activateReplace();
    bool doReplace(bool forAll = false);
    void doReplaceAll();

protected slots:
    void searchLineEditTextChanged(const QString &arg1);
    void onActionTriggered(PopupMenu::SearchMode searchMode, bool matchCaseSensitive = false);
};
