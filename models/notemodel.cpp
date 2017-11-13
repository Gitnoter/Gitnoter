#include "notemodel.h"
#include "tools.h"
#include "globals.h"
#include "notelistcellwidget.h"

#include <json.h>
#include <html.h>

#include <QUrl>
#include <QMimeDatabase>
#include <QDateTime>
#include <QDebug>

NoteModel::NoteModel(const QString noteText)
{
    clear();
    setNoteText(noteText);
}

NoteModel::NoteModel(const QString textPath, const QString dataPath)
{
    setNoteText(Tools::readerFile(textPath));
    setNoteData(Tools::readerFile(dataPath));
}

QString NoteModel::getNoteDir()
{
    if (mUuid.isEmpty()) {
        setUuid();
    }

    return QDir(Globals::repoNoteTextPath).filePath(getUuid());
}


QString NoteModel::getMarkdownHtml(int maxImageWidth, bool forExport, bool decrypt, bool base64Images)
{
    // get the decrypted note text (or the normal note text if there isn't any)
    hoedown_renderer *renderer =
            hoedown_html_renderer_new(HOEDOWN_HTML_USE_XHTML, 32);

    // we want to show quotes in the html, so we don't translate them into
    // `<q>` tags
    // HOEDOWN_EXT_MATH and HOEDOWN_EXT_MATH_EXPLICIT don't seem to do anything
    hoedown_extensions extensions =
            (hoedown_extensions) ((HOEDOWN_EXT_BLOCK | HOEDOWN_EXT_SPAN |
                                   HOEDOWN_EXT_MATH_EXPLICIT) & ~HOEDOWN_EXT_QUOTE);
    hoedown_document *document = hoedown_document_new(renderer, extensions, 32);

    QString windowsSlash = "";

#ifdef Q_OS_WIN32
    // we need an other slash for Windows
    windowsSlash = "/";
#endif

    // parse for relative file urls and make them absolute
    // (for example to show images under the note path)
//    str.replace(
//            QRegularExpression("([\\(<])file:\\/\\/([^\\/].+?)([\\)>])"),
//            "\\1file://" + windowsSlash + QRegularExpression::escape(notesPath)
//            + "/\\2\\3");

    QRegularExpressionMatchIterator i;

    // try to replace file links like <my-note.md> to note links
    // this is a "has not '\w+:\/\/' in it" regular expression
    // see: http://stackoverflow.com/questions/406230/regular-expression-to-match-line-that-doesnt-contain-a-word
    // TODO: maybe we could do that per QTextBlock to check if it's done in comment block?
    // Important: The `\n` is needed to not crash under Windows if there is just
    //            an opening `<` and a lot of other text after it
//    i = QRegularExpression("<(((?!\\w+:\\/\\/)[^<>\n])+)>").globalMatch(str);

//    while (i.hasNext()) {
//        QRegularExpressionMatch match = i.next();
//        QString fileLink = match.captured(1);
//        QString noteUrl = Note::getNoteURLFromFileName(fileLink);
//        qDebug() << fileLink << " fileLink " << noteUrl;

//        // try to load the note to check if it really exists
//        Note note = Note::fetchByFileName(fileLink);

//        if (!noteUrl.isEmpty() && note.exists()) {
//            str.replace(match.captured(0),
//                        "[" + fileLink + "](" + noteUrl + ")");
//        }
//    }

//    // try to replace file links like [my note](my-note.md) to note links
//    // we are using `{1,500}` instead of `+` because there were crashes with
//    // regular expressions running wild
//    i = QRegularExpression(
//            "\\[(.+?)\\]\\((((?!\\w+:\\/\\/)[^<>]){1,500}?)\\)")
//            .globalMatch(str);

//    while (i.hasNext()) {
//        QRegularExpressionMatch match = i.next();
//        QString fileText = match.captured(1);
//        QString fileLink = match.captured(2);
//        QString noteUrl = Note::getNoteURLFromFileName(fileLink);

//        // try to load the note to check if it really exists
//        Note note = Note::fetchByFileName(fileLink);

//        if (!noteUrl.isEmpty() && note.exists()) {
//            str.replace(match.captured(0),
//                        "[" + fileText + "](" + noteUrl + ")");
//        }
//    }

    unsigned char *sequence = (unsigned char *) qstrdup(
            mNoteText.toUtf8().constData());
    qint64 length = strlen((char *) sequence);

    // return an empty string if the note is empty
    if (length == 0) {
        return "";
    }

    hoedown_buffer *html = hoedown_buffer_new(length);

    // render markdown html
    hoedown_document_render(document, html, sequence, length);

    // get markdown html
    QString result = QString::fromUtf8((char *) html->data, html->size);

//    qDebug() << __func__ << " - 'result': " << result;

    /* Cleanup */
    free(sequence);
    hoedown_buffer_free(html);

    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

//    QSettings(settings);
//    QString fontString = settings.value("MainWindow/noteTextView.code.font")
//            .toString();
    QString fontString = "";

    // set the stylesheet for the <code> blocks
    QString codeStyleSheet = "";
    if (fontString != "") {
        // set the note text view font
        QFont font;
        font.fromString(fontString);

        // add the font for the code block
        codeStyleSheet = QString(
                "pre, code { %1; }").arg(encodeCssFont(font));
    }

//    bool darkModeColors = settings.value("darkModeColors").toBool();
    bool darkModeColors = false;

    QString codeBackgroundColor = darkModeColors ? "#444444" : "#f1f1f1";

    // do some more code formatting
    codeStyleSheet += QString(
            "pre, code { padding: 16px; overflow: auto;"
                    " line-height: 1.45em; background-color: %1;"
                    " border-radius: 3px; }").arg(codeBackgroundColor);

    // remove double code blocks
    result.replace("<pre><code>", "<pre>")
            .replace("</code></pre>", "</pre>");

    // correct the strikeout tag
    result.replace(QRegularExpression("<del>([^<]+)<\\/del>"), "<s>\\1</s>");
//    bool rtl = settings.value("MainWindow/noteTextView.rtl").toBool();
    bool rtl = false;
    QString rtlStyle = rtl ? "body {text-align: right; direction: rtl;}" : "";

    if (forExport) {
        // get defined body font from settings
//        QString bodyFontString = settings.value("MainWindow/noteTextView.font")
//                .toString();
        QString bodyFontString = "";

        // create export stylesheet
        QString exportStyleSheet = "";
        if (bodyFontString != "") {
            QFont bodyFont;
            bodyFont.fromString(bodyFontString);

            exportStyleSheet = QString(
                    "body { %1; }").arg(encodeCssFont(bodyFont));
        }

        result = QString("<html><head><meta charset=\"utf-8\"/><style>"
                                 "h1 { margin: 5px 0 20px 0; }"
                                 "h2, h3 { margin: 10px 0 15px 0; }"
                                 "img { max-width: 100%; }"
                                 "a { color: #FF9137; text-decoration: none; } %1 %2 %4"
                                 "</style></head><body>%3</body></html>")
                .arg(codeStyleSheet, exportStyleSheet, result, rtlStyle);
    } else {
        // for preview
        result = QString("<html><head><style>"
                                 "h1 { margin: 5px 0 20px 0; }"
                                 "h2, h3 { margin: 10px 0 15px 0; }"
                                 "a { color: #FF9137; text-decoration: none; } %1 %3"
                                 "</style></head><body>%2</body></html>")
                .arg(codeStyleSheet, result, rtlStyle);
    }

    // check if width of embedded local images is too high
    QRegularExpression re("<img src=\"(file:\\/\\/[^\"]+)\"");
    i = re.globalMatch(result);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString fileUrl = match.captured(1);
        QString fileName = QUrl(fileUrl).toLocalFile();
        QImage image(fileName);

        if (forExport) {
            result.replace(
                    QRegularExpression("<img src=\"file:\\/\\/" +
                                       QRegularExpression::escape(windowsSlash +
                                                                  fileName) +
                                       "\""),
                    QString("<img src=\"file://%2\"").arg(windowsSlash +
                                                          fileName));
        } else {
            // for preview
            // cap the image width at 980px or the note text view width
            if (image.width() > maxImageWidth) {
                result.replace(
                        QRegularExpression("<img src=\"file:\\/\\/" +
                                           QRegularExpression::escape
                                                   (windowsSlash + fileName) +
                                           "\""),
                        QString("<img width=\"%1\" src=\"file://%2\"").arg(
                                QString::number(maxImageWidth), windowsSlash +
                                                                fileName));
            }
        }

        // encode the image base64
        if (base64Images) {
            QFile file(fileName);

            if (!file.open(QIODevice::ReadOnly)) {
                qWarning() << QObject::tr("Could not read image file: %1")
                        .arg(fileName);

                continue;
            }

            QMimeDatabase db;
            QMimeType type = db.mimeTypeForFile(file.fileName());
            QByteArray ba = file.readAll();

            result.replace(
                    QRegularExpression(
                            "<img(.+?)src=\"" + QRegularExpression::escape(
                                    fileUrl) + "\""),
                    QString("<img\\1src=\"data:%1;base64,%2\"").arg(
                            type.name(), QString(ba.toBase64())));
        }
    }

    return result;
}

QString NoteModel::encodeCssFont(const QFont& refFont) {
//-----------------------------------------------------------------------
// This function assembles a CSS Font specification string from
// a QFont. This supports most of the QFont attributes settable in
// the Qt 4.8 and Qt 5.3 QFontDialog.
//
// (1) Font Family
// (2) Font Weight (just bold or not)
// (3) Font Style (possibly Italic or Oblique)
// (4) Font Size (in either pixels or points)
// (5) Decorations (possibly Underline or Strikeout)
//
// Not supported: Writing System (e.g. Latin).
//
// See the corresponding decode function, below.
// QFont decodeCssFontString (const QString cssFontStr)
//-----------------------------------------------------------------------

    QStringList fields; // CSS font attribute fields

// ***************************************************
// *** (1) Font Family: Primary plus Substitutes ***
// ***************************************************

    const QString family = refFont.family();

// NOTE [9-2014, Qt 4.8.6]: This isn't what I thought it was. It
// does not return a list of "fallback" font faces (e.g. Georgia,
// Serif for "Times New Roman"). In my testing, this is always
// returning an empty list.
//
    QStringList famSubs = QFont::substitutes (family);

    if (!famSubs.contains (family))
        famSubs.prepend (family);

    static const QChar DBL_QUOT ('"');
    const int famCnt = famSubs.count();
    QStringList famList;
    for (int inx = 0; inx < famCnt; ++inx)
    {
// Place double quotes around family names having space characters,
// but only if double quotes are not already there.
//
        const QString fam = famSubs [inx];
        if (fam.contains (' ') && !fam.startsWith (DBL_QUOT))
            famList << (DBL_QUOT + fam + DBL_QUOT);
        else
            famList << fam;
    }

    const QString famStr = QString ("font-family: ") + famList.join (", ");
    fields << famStr;

// **************************************
// *** (2) Font Weight: Bold or Not ***
// **************************************

    const bool bold = refFont.bold();
    if (bold)
        fields << "font-weight: bold";

// ****************************************************
// *** (3) Font Style: possibly Italic or Oblique ***
// ****************************************************

    const QFont::Style style = refFont.style();
    switch (style)
    {
        case QFont::StyleNormal: break;
        case QFont::StyleItalic: fields << "font-style: italic"; break;
        case QFont::StyleOblique: fields << "font-style: oblique"; break;
    }

// ************************************************
// *** (4) Font Size: either Pixels or Points ***
// ************************************************

    const double sizeInPoints = refFont.pointSizeF(); // <= 0 if not defined.
    const int sizeInPixels = refFont.pixelSize(); // <= 0 if not defined.
    if (sizeInPoints > 0.0)
        fields << QString ("font-size: %1pt") .arg (sizeInPoints);
    else if (sizeInPixels > 0)
        fields << QString ("font-size: %1px") .arg (sizeInPixels);

// ***********************************************
// *** (5) Decorations: Underline, Strikeout ***
// ***********************************************

    const bool underline = refFont.underline();
    const bool strikeOut = refFont.strikeOut();

    if (underline && strikeOut)
        fields << "text-decoration: underline line-through";
    else if (underline)
        fields << "text-decoration: underline";
    else if (strikeOut)
        fields << "text-decoration: line-through";

    const QString cssFontStr = fields.join ("; ");
    return cssFontStr;
}

QString NoteModel::getStringFormTagList() const
{
    QString tagsString = "";
    for (auto &&item : tagList) {
        tagsString += item + Globals::tagSplit;
    }
    tagsString.chop(Globals::tagSplit.length());

    return tagsString;
}

void NoteModel::setNoteData(const QString &noteData)
{
    QtJson::JsonObject result = QtJson::parse(noteData).toMap();

    mUuid = result["uuid"].toString();
    mCreateDate = result["createDate"].toInt();
    mUpdateDate = result["updateDate"].toInt();
    category = result["category"].toString();
    isDelete = result["isDelete"].toInt();

    tagList.clear();
    QtJson::JsonArray tagListArray = result["tagList"].toList();
    for (int i = 0; i < tagListArray.length(); ++i) {
        tagList.append(tagListArray[i].toString());
    }
}

QString NoteModel::getNoteData() const
{
    QtJson::JsonObject contributor;

    contributor["uuid"] = mUuid;
    contributor["createDate"] = mCreateDate;
    contributor["updateDate"] = mUpdateDate;
    contributor["category"] = category;
    contributor["isDelete"] = isDelete;

    QtJson::JsonArray tagListArray;
    for (int i = 0; i < tagList.length(); ++i) {
        tagListArray.append(tagList[i]);
    }
    contributor["tagList"] = tagListArray;

    return QString(QtJson::serialize(contributor));
}

void NoteModel::clear()
{
    setUuid();
    setNoteText();
    setCreateDate(0);
    setUpdateDate(0);
    setTagList();
    setCategory();
    setIsDelete();
}

void NoteModel::saveNoteToLocal()
{
    setUpdateDate(0);
    QString path = getNoteDir();
    Tools::createMkDir(path);
    Tools::writerFile(QDir(path).filePath(Globals::noteTextFileName), getNoteText());
    Tools::writerFile(QDir(path).filePath(Globals::noteDataFileName), getNoteData());
}

void NoteModel::saveNoteTextToLocal()
{
    setUpdateDate(0);
    QString path = getNoteDir();
    Tools::createMkDir(path);
    Tools::writerFile(QDir(path).filePath(Globals::noteTextFileName), getNoteText());
}

void NoteModel::saveNoteDataToLocal()
{
    setUpdateDate(0);
    QString path = getNoteDir();
    Tools::createMkDir(path);
    Tools::writerFile(QDir(path).filePath(Globals::noteDataFileName), getNoteData());
}

QString NoteModel::getUuid()
{
    return mUuid;
}

void NoteModel::setCreateDate(const QString createDate)
{
    mCreateDate = createDate.isEmpty() ? (int) QDateTime::currentSecsSinceEpoch()
                                       : Tools::timestampFromDateTime(createDate);
}

void NoteModel::setCreateDate(int createDate)
{
    mCreateDate = createDate == 0 ? (int) QDateTime::currentSecsSinceEpoch() : createDate;
}

void NoteModel::setUpdateDate(const QString updateDate)
{
    mUpdateDate = updateDate.isEmpty() ? (int) QDateTime::currentSecsSinceEpoch()
                                       : Tools::timestampFromDateTime(updateDate);
}

void NoteModel::setUpdateDate(int updateDate)
{
    mUpdateDate = updateDate == 0 ? (int) QDateTime::currentSecsSinceEpoch() : updateDate;
}

void NoteModel::setNoteText(QString body)
{
    mNoteText = body;
}

void NoteModel::setUuid(QString uuid)
{
    mUuid = uuid.isEmpty() ? Tools::getUuid() : uuid;
}

int NoteModel::getCreateDate()
{
    return mCreateDate;
}

int NoteModel::getUpdateDate()
{
    return mUpdateDate;
}

const QString NoteModel::getCreateDateString()
{
    return Tools::timestampToDateTime(mCreateDate);
}

const QString NoteModel::getUpdateDateString()
{
    return Tools::timestampToDateTime(mUpdateDate);
}

QString NoteModel::getNoteText()
{
    return mNoteText;
}

const QStringList &NoteModel::getTagList() const
{
    return tagList;
}

void NoteModel::setTagList(const QStringList &tagList)
{
    NoteModel::tagList = tagList;
}

const QString &NoteModel::getCategory() const
{
    return category;
}

void NoteModel::setCategory(const QString &category)
{
    NoteModel::category = category;
}

const QString &NoteModel::getTitle() const
{
    return mTitle;
}

void NoteModel::setTitle(const QString &title)
{
    NoteModel::mTitle = title;
}

int NoteModel::getIsDelete() const
{
    return isDelete;
}

void NoteModel::setIsDelete(int isDelete)
{
    NoteModel::isDelete = isDelete;
}