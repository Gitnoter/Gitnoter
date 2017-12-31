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

#include "qplaintexteditsearchwidget.h"
#include "ui_qplaintexteditsearchwidget.h"
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QAction>


PopupMenu::PopupMenu(QWidget *parent, PopupMenu::SearchMode mode, bool matchCaseSensitive) :
        QMenu(parent)
{
    setupUi(parent);
    setSearchMode(mode);
    action_matchCaseSensitive->setChecked(matchCaseSensitive);
}

void PopupMenu::onActionPlainTextTriggered(bool triggered)
{
    seActionChecked(action_plainText);
    mSearchMode = PopupMenu::PlainTextMode;
    emit actionTriggered(mSearchMode, triggered);
}

void PopupMenu::onActionWholeWordsTriggered(bool triggered)
{
    seActionChecked(action_wholeWords);
    mSearchMode = PopupMenu::WholeWordsMode;
    emit actionTriggered(mSearchMode, triggered);
}

void PopupMenu::onActionRegularExpressionTriggered(bool triggered)
{
    seActionChecked(action_regularExpression);
    mSearchMode = PopupMenu::RegularExpressionMode;
    emit actionTriggered(mSearchMode, triggered);
}

void PopupMenu::onActionMatchCaseSensitiveTriggered(bool triggered)
{
    emit actionTriggered(mSearchMode, triggered);
}

void PopupMenu::seActionChecked(QAction *action)
{
    action_plainText->setChecked(false);
    action_wholeWords->setChecked(false);
    action_regularExpression->setChecked(false);

    action->setChecked(true);
}

void PopupMenu::setSearchMode(PopupMenu::SearchMode mode)
{
    mSearchMode = mode;
    switch (mode) {
        case PopupMenu::PlainTextMode: seActionChecked(action_plainText);break;
        case PopupMenu::WholeWordsMode: seActionChecked(action_wholeWords);break;
        case PopupMenu::RegularExpressionMode: seActionChecked(action_regularExpression);break;
    default:break;
    }
}

bool PopupMenu::matchCaseSensitiveChecked()
{
    return action_matchCaseSensitive->isChecked();
}

void PopupMenu::setupUi(QWidget *parent)
{
    action_plainText = new QAction(tr(u8"纯文本"), this);
    action_wholeWords = new QAction(tr(u8"全词匹配"), this);
    action_regularExpression = new QAction(tr(u8"正则匹配"), this);
    action_matchCaseSensitive = new QAction(tr(u8"是否区分大小写"), this);

    action_plainText->setObjectName(QStringLiteral("action_plainText"));
    action_wholeWords->setObjectName(QStringLiteral("action_wholeWords"));
    action_regularExpression->setObjectName(QStringLiteral("action_regularExpression"));
    action_matchCaseSensitive->setObjectName(QStringLiteral("action_matchCaseSensitive"));

    action_plainText->setCheckable(true);
    action_wholeWords->setCheckable(true);
    action_regularExpression->setCheckable(true);
    action_matchCaseSensitive->setCheckable(true);

    addAction(action_plainText);
    addAction(action_wholeWords);
    addAction(action_regularExpression);
    addSeparator();
    addAction(action_matchCaseSensitive);

    connect(action_plainText, SIGNAL(triggered(bool)), this, SLOT(onActionPlainTextTriggered(bool)));
    connect(action_wholeWords, SIGNAL(triggered(bool)), this, SLOT(onActionWholeWordsTriggered(bool)));
    connect(action_regularExpression, SIGNAL(triggered(bool)), this, SLOT(onActionRegularExpressionTriggered(bool)));
    connect(action_matchCaseSensitive, SIGNAL(triggered(bool)), this, SLOT(onActionMatchCaseSensitiveTriggered(bool)));
}

QPlainTextEditSearchWidget::QPlainTextEditSearchWidget(QPlainTextEdit *parent) :
    QWidget(parent),
    ui(new Ui::QPlainTextEditSearchWidget)
{
    ui->setupUi(this);
    _textEdit = parent;
    hide();

    QAction *searchAction = new QAction(QIcon(":/images/icon-search.png"), QString());
    mPopupMenu = new PopupMenu(this);
    searchAction->setMenu(mPopupMenu);
    ui->searchLineEdit->addAction(searchAction, QLineEdit::LeadingPosition);

    QObject::connect(ui->closeButton, SIGNAL(clicked()),
                     this, SLOT(deactivate()));
    QObject::connect(ui->searchLineEdit, SIGNAL(textChanged(const QString &)),
                     this, SLOT(searchLineEditTextChanged(const QString &)));
    QObject::connect(ui->searchDownButton, SIGNAL(clicked()),
                     this, SLOT(doSearchDown()));
    QObject::connect(ui->searchUpButton, SIGNAL(clicked()),
                     this, SLOT(doSearchUp()));
    QObject::connect(ui->replaceToggleCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(setReplaceMode(bool)));
    QObject::connect(ui->replaceButton, SIGNAL(clicked()),
                     this, SLOT(doReplace()));
    QObject::connect(ui->replaceAllButton, SIGNAL(clicked()),
                     this, SLOT(doReplaceAll()));
    QObject::connect(mPopupMenu, SIGNAL(actionTriggered(PopupMenu::SearchMode, bool)),
                     this, SLOT(onActionTriggered(PopupMenu::SearchMode, bool)));

    installEventFilter(this);
    ui->searchLineEdit->installEventFilter(this);
    ui->replaceLineEdit->installEventFilter(this);

#ifdef Q_OS_MAC
    // set the spacing to 8 for OS X
//    layout()->setSpacing(8);
//    ui->buttonFrame->layout()->setSpacing(9);

    // set the margin to 0 for the top buttons for OS X
    QString buttonStyle = "QPushButton {margin: 0}";
    ui->closeButton->setStyleSheet(buttonStyle);
    ui->searchDownButton->setStyleSheet(buttonStyle);
    ui->searchUpButton->setStyleSheet(buttonStyle);
#endif
}

QPlainTextEditSearchWidget::~QPlainTextEditSearchWidget() {
    delete ui;
}

void QPlainTextEditSearchWidget::activate(bool replace) {
    setReplaceMode(replace);
    show();

    // preset the selected text as search text if there is any and there is no
    // other search text
    QString selectedText = _textEdit->textCursor().selectedText();
    if (!selectedText.isEmpty() && ui->searchLineEdit->text().isEmpty()) {
        ui->searchLineEdit->setText(selectedText);
    }

    ui->searchLineEdit->setFocus();
    ui->searchLineEdit->selectAll();
    doSearchDown();
}

void QPlainTextEditSearchWidget::activateReplace() {
    // replacing is prohibited if the text edit is readonly
    if (_textEdit->isReadOnly()) {
        return;
    }

    ui->searchLineEdit->setText(_textEdit->textCursor().selectedText());
    ui->searchLineEdit->selectAll();
    activate();
    setReplaceMode(true);
}

void QPlainTextEditSearchWidget::deactivate() {
    hide();
    _textEdit->setFocus();
}

void QPlainTextEditSearchWidget::setReplaceMode(bool enabled) {
    ui->replaceFrame->setVisible(enabled);
    ui->buttonFrame->setVisible(enabled);
    ui->replaceToggleCheckBox->setChecked(enabled);
}

bool QPlainTextEditSearchWidget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Escape) {
            deactivate();
            return true;
        } else if ((keyEvent->modifiers().testFlag(Qt::ShiftModifier) &&
                  (keyEvent->key() == Qt::Key_Return)) ||
                 (keyEvent->key() == Qt::Key_Up)) {
            doSearchUp();
            return true;
        } else if ((keyEvent->key() == Qt::Key_Return) ||
                 (keyEvent->key() == Qt::Key_Down)) {
            doSearchDown();
            return true;
        } else if (keyEvent->key() == Qt::Key_F3) {
            doSearch(!keyEvent->modifiers().testFlag(Qt::ShiftModifier));
            return true;
        }

//        if ((obj == ui->replaceLineEdit) && (keyEvent->key() == Qt::Key_Tab)
//                && ui->replaceToggleCheckBox->isChecked()) {
//            ui->replaceLineEdit->setFocus();
//        }

        return false;
    }

    return QWidget::eventFilter(obj, event);
}

void QPlainTextEditSearchWidget::searchLineEditTextChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    doSearchDown();
}

void QPlainTextEditSearchWidget::doSearchUp() {
    doSearch(false);
}

void QPlainTextEditSearchWidget::doSearchDown() {
    doSearch(true);
}

bool QPlainTextEditSearchWidget::doReplace(bool forAll) {
    if (_textEdit->isReadOnly()) {
        return false;
    }

    QTextCursor c = _textEdit->textCursor();

    if (!forAll && c.selectedText().isEmpty()) {
        return false;
    }

    PopupMenu::SearchMode searchMode = mPopupMenu->searchModel();
    if (searchMode == PopupMenu::RegularExpressionMode) {
        QString text = c.selectedText();
        text.replace(QRegExp(ui->searchLineEdit->text()),
                             ui->replaceLineEdit->text());
        c.insertText(text);
    } else {
        c.insertText(ui->replaceLineEdit->text());
    }

    if (!forAll) {
        int position = c.position();

        if (!doSearch(true)) {
            // restore the last cursor position if text wasn't found any more
            c.setPosition(position);
            _textEdit->setTextCursor(c);
        }
    }

    return true;
}

void QPlainTextEditSearchWidget::doReplaceAll() {
    if (_textEdit->isReadOnly()) {
        return;
    }

    // start at the top
    _textEdit->moveCursor(QTextCursor::Start);

    // replace until everything to the bottom is replaced
    while (doSearch(true, false) && doReplace(true)) {}
}

/**
 * @brief Searches for text in the text edit
 * @returns true if found
 */
bool QPlainTextEditSearchWidget::doSearch(bool searchDown, bool allowRestartAtTop) {
    QString text = ui->searchLineEdit->text();

    if (text == "") {
        ui->searchLineEdit->setStyleSheet("");
        return false;
    }

    PopupMenu::SearchMode searchMode = mPopupMenu->searchModel();

    QFlags<QTextDocument::FindFlag> options = searchDown ?
                                              QTextDocument::FindFlag(0)
                                              : QTextDocument::FindBackward;
    if (searchMode == PopupMenu::WholeWordsMode) {
        options |= QTextDocument::FindWholeWords;
    }

    if (mPopupMenu->matchCaseSensitiveChecked()) {
        options |= QTextDocument::FindCaseSensitively;
    }

    bool found;
    if (searchMode == PopupMenu::RegularExpressionMode) {
        found = _textEdit->find(QRegExp(text), options);
    } else {
        found = _textEdit->find(text, options);
    }

    // start at the top (or bottom) if not found
    if (!found && allowRestartAtTop) {
        _textEdit->moveCursor(
                searchDown ? QTextCursor::Start : QTextCursor::End);
        found = _textEdit->find(text, options);
    }

    QRect rect = _textEdit->cursorRect();
    QMargins margins = _textEdit->layout()->contentsMargins();
    int searchWidgetHotArea = _textEdit->height() - this->height();
    int marginBottom = (rect.y() > searchWidgetHotArea) ? (this->height() + 10)
                                                        : 0;

    // move the search box a bit up if we would block the search result
    if (margins.bottom() != marginBottom) {
        margins.setBottom(marginBottom);
        _textEdit->layout()->setContentsMargins(margins);
    }

    return found;
}

bool QPlainTextEditSearchWidget::isReplace()
{
    return ui->replaceToggleCheckBox->isChecked();
}

void QPlainTextEditSearchWidget::onActionTriggered(PopupMenu::SearchMode searchMode, bool matchCaseSensitive)
{
    doSearch(false);
}


