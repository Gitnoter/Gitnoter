#include "ui_importnotedialog.h"

#include "importnotedialog.h"
#include "tools.h"
#include "mainwindow.h"

#include <QFileDialog>

ImportNoteDialog::ImportNoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportNoteDialog)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
}

ImportNoteDialog::~ImportNoteDialog()
{
    delete ui;
}

void ImportNoteDialog::init()
{
    QFileDialog *dialog = new QFileDialog();
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setNameFilter(tr("Enex files") + " (*.enex)");
    dialog->setWindowTitle(tr("Select Evernote enex file to import"));
    dialog->show();

    connect(dialog, &QFileDialog::fileSelected, [&](const QString &fileName) {
        if (fileName.isEmpty()) { return; }
        ui->buttonBox->setEnabled(false);
        open();
        QString data = Tools::readerFile(fileName);
        QCoreApplication::processEvents();
        initProgressBar(data);
        QCoreApplication::processEvents();
        importNotes(data);
    });

    qDebug() << __func__;
}

void ImportNoteDialog::initProgressBar(QString data)
{
    QXmlQuery query;
    query.setFocus(data);
    query.setQuery("en-export/note");
    int count = 0;

    QXmlResultItems result;
    if (query.isValid()) {
        query.evaluateTo(&result);

        while (!result.next().isNull()) {
            count++;
        }
    }

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(count);
}


void ImportNoteDialog::importNotes(QString data)
{
    QXmlQuery query;
    query.setFocus(data);
    query.setQuery("en-export/note");

    QXmlResultItems result;
    if (query.isValid()) {
        query.evaluateTo(&result);

        while (!result.next().isNull()) {
            NoteModel *noteModel = new NoteModel();
            noteModel->saveNoteToLocal();

            query.setFocus(result.current());

            QString createDate;
            query.setQuery("created/text()");
            query.evaluateTo(&createDate);
            createDate = createDate.trimmed();

            QString updateDate;
            query.setQuery("updated/text()");
            query.evaluateTo(&updateDate);
            updateDate = updateDate.trimmed();

            QString title;
            query.setQuery("title/text()");
            query.evaluateTo(&title);
            title = title.trimmed();

            QString content;
            query.setQuery("content/text()");
            query.evaluateTo(&content); // content seems to be html encoded

            content.replace("\\\"", "\"");

            content = decodeHtmlEntities(content);

            // add a linebreak instead of div-containers
            content.replace(QRegularExpression("<\\/div>"), "\n");

            content = NoteModel::htmlToMarkdown(content);       // convert html tags to markdown
            content = importImages(noteModel, content, query);  // import images
            content = importAttachments(noteModel, content, query); // import attachments

            // remove all html tags
            content.remove(QRegularExpression("<.+?>"));

            // remove multiple \n
            content.replace(QRegularExpression("\n\n+"), "\n\n");
            content.replace(QRegularExpression("\n\n\\s+"), "\n\n");

#ifdef Q_OS_WIN32
            // removing or replacing some characters that are asking for
            // troubles
            title.replace("—", "-");
            title.replace("|", "-");
            title.replace("/", "-");
            title.replace("\\", "-");
            title.remove("#");
            title.remove("?");
            title.remove("'");
            title.remove("\"");
#endif

            const QString noteText = "# " + title + "\n\n" + content;

            noteModel->setNoteText(noteText);
            noteModel->setCreateDate(Tools::timestampFromDateTime(createDate, "yyyyMMddThhmmssZ"));
            noteModel->setUpdateDate(Tools::timestampFromDateTime(updateDate, "yyyyMMddThhmmssZ"));
            noteModel->saveNoteToLocal();

            ui->progressBar->setValue(ui->progressBar->value() + 1);
            QCoreApplication::processEvents();

            if (ui->progressBar->value() == ui->progressBar->maximum()) {
                ui->buttonBox->setEnabled(true);
                ((MainWindow *) parent())->init();
            }
        }

        if (result.hasError()) {
            qWarning() << "Error importing notes";
        }
    }
}

QString ImportNoteDialog::importImages(NoteModel *noteModel, QString content, QXmlQuery query)
{
    query.setQuery("resource");

    QXmlResultItems result;
    QHash<QString, MediaFileData> mediaFileDataHash;

    if (!query.isValid()) {
        return content;
    }

    query.evaluateTo(&result);
    QRegularExpressionMatch match;

    while (!result.next().isNull()) {
        QString objectType;
        QString suffix;
        query.setFocus(result.current());

        // parse the mime data of the object
        QString mime;
        query.setQuery("mime/text()");
        query.evaluateTo(&mime);

        QRegularExpression mimeExpression(
                "(.+)?\\/(.+)?", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch mimeMatch = mimeExpression.match(mime);

        if (mimeMatch.hasMatch()) {
            objectType = mimeMatch.captured(1);
            suffix = mimeMatch.captured(2);
        }

        // we only want to import images
        if (objectType != "image") {
            continue;
        }

        // check the recognition attribute for an object type
        QString recognition;
        query.setQuery("recognition/text()");
        query.evaluateTo(&recognition);

        recognition.replace("\\\"", "\"");

        match = QRegularExpression(
                "objID=\"(.+?)\"",
                QRegularExpression::CaseInsensitiveOption)
                .match(recognition);

        QString objectId;

        if (match.hasMatch()) {
            // get the object id of the resource to match it with the
            // en-media hash
            objectId = match.captured(1);
        }

        QString data;
        query.setQuery("data/text()");
        query.evaluateTo(&data);

        // if there is no object id set we generate the hash ourselves
        if (objectId.isEmpty()) {
            objectId = QString(QCryptographicHash::hash(
                    QByteArray::fromBase64(data.toLatin1()),
                    QCryptographicHash::Md5).toHex());
        }

        // store data in the QHash
        mediaFileDataHash[objectId].data = data;
        mediaFileDataHash[objectId].suffix = suffix;
    }

    if (mediaFileDataHash.count() == 0) {
        return content;
    }

    // match image tags
    QRegularExpression re("<en-media.+?type=\"image/.+?\".*?>",
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(content);
    QStringList importedObjectIds;

    // find all images
    while (i.hasNext()) {
        QRegularExpressionMatch imageMatch = i.next();
        QString imageTag = imageMatch.captured(0);

        // check for the hash
        QRegularExpression re2("hash=\"(.+?)\"",
                               QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch hashMatch = re2.match(imageTag);

        if (!hashMatch.hasMatch()) {
            continue;
        }

        QString objectId = hashMatch.captured(1);

        if (!mediaFileDataHash.contains(objectId)) {
            continue;
        }

        importedObjectIds << objectId;

        // get the base64 encoded image
        MediaFileData mediaFileData = mediaFileDataHash[objectId];

        // get the markdown code for the image file data entry
        QString markdownCode = getMarkdownForMediaFileData(noteModel, mediaFileData);

        if (!markdownCode.isEmpty()) {
            // replace image tag with markdown code
            content.replace(imageTag, markdownCode);
        }
    }

    // get the image markdown code for missing images
    QHashIterator<QString, MediaFileData> hashIterator(mediaFileDataHash);
    while (hashIterator.hasNext()) {
        hashIterator.next();
        QString objectId = hashIterator.key();
        if (importedObjectIds.contains(objectId)) {
            continue;
        }

        MediaFileData mediaFileData = hashIterator.value();

        // get the markdown code for the image file data entry
        QString markdownCode = getMarkdownForMediaFileData(noteModel, mediaFileData);

        content += "\n" + markdownCode;
    }

    return content;
}

QString ImportNoteDialog::importAttachments(NoteModel *noteModel, QString content, QXmlQuery query)
{
    query.setQuery("resource");

    QXmlResultItems result;
    QHash<QString, MediaFileData> mediaFileDataHash;

    if (!query.isValid()) {
        return content;
    }

    query.evaluateTo(&result);
    QRegularExpressionMatch match;

    while (!result.next().isNull()) {
        QString objectType;
        query.setFocus(result.current());

        // parse the mime data of the object
        QString mime;
        query.setQuery("mime/text()");
        query.evaluateTo(&mime);

        QRegularExpression mimeExpression(
                "(.+)?\\/(.+)?", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch mimeMatch = mimeExpression.match(mime);

        if (mimeMatch.hasMatch()) {
            objectType = mimeMatch.captured(1);
        }

        // we don't want to import images
        if (objectType == "image") {
            continue;
        }

        // check the recognition attribute for an object type
        QString recognition;
        query.setQuery("recognition/text()");
        query.evaluateTo(&recognition);

        recognition.replace("\\\"", "\"");

        match = QRegularExpression(
                "objID=\"(.+?)\"",
                QRegularExpression::CaseInsensitiveOption)
                .match(recognition);

        QString objectId;

        if (match.hasMatch()) {
            // get the object id of the resource to match it with the
            // en-media hash
            objectId = match.captured(1);
        }

        QString data;
        query.setQuery("data/text()");
        query.evaluateTo(&data);

        // if there is no object id set we generate the hash ourselves
        if (objectId.isEmpty()) {
            objectId = QString(QCryptographicHash::hash(
                    QByteArray::fromBase64(data.toLatin1()),
                    QCryptographicHash::Md5).toHex());
        }

        // find the filename
        QString fileName;
        query.setQuery("resource-attributes/file-name/text()");
        query.evaluateTo(&fileName);
        fileName = fileName.trimmed();

        // get the file suffix from the filename
        QFileInfo fileInfo(fileName);
        QString suffix = fileInfo.suffix();

        // store data in the QHash
        mediaFileDataHash[objectId].data = data;
        mediaFileDataHash[objectId].suffix = suffix;
        mediaFileDataHash[objectId].fileName = fileName;
    }

    if (mediaFileDataHash.count() == 0) {
        return content;
    }

    // match media tags
    QRegularExpression re("<en-media.+?type=\".+?\".*?>",
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(content);
    QStringList importedObjectIds;

    // find all files
    while (i.hasNext()) {
        QRegularExpressionMatch imageMatch = i.next();
        QString mediaTag = imageMatch.captured(0);

        // check for the hash
        QRegularExpression re2("hash=\"(.+?)\"",
                               QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch hashMatch = re2.match(mediaTag);

        if (!hashMatch.hasMatch()) {
            continue;
        }

        QString objectId = hashMatch.captured(1);

        if (!mediaFileDataHash.contains(objectId)) {
            continue;
        }

        importedObjectIds << objectId;

        // get the base64 encoded file
        MediaFileData mediaFileData = mediaFileDataHash[objectId];

        // get the markdown code for the file data entry
        QString markdownCode = getMarkdownForAttachmentFileData(noteModel, mediaFileData);

        if (!markdownCode.isEmpty()) {
            // replace media tag with markdown code
            content.replace(mediaTag, markdownCode);
        }
    }

    // get the file markdown code for missing attachments
    QHashIterator<QString, MediaFileData> hashIterator(mediaFileDataHash);
    while (hashIterator.hasNext()) {
        hashIterator.next();
        QString objectId = hashIterator.key();
        if (importedObjectIds.contains(objectId)) {
            continue;
        }

        MediaFileData mediaFileData = hashIterator.value();

        // get the markdown code for the file data entry
        QString markdownCode = getMarkdownForAttachmentFileData(noteModel, mediaFileData);

        content += "\n" + markdownCode;
    }

    return content;
}

QString ImportNoteDialog::getMarkdownForMediaFileData(NoteModel *noteModel, MediaFileData &mediaFileData)
{
    const QString saveName = Tools::getShortUuid();
    const QString savePath = noteModel->getNoteDir() + QDir::separator() + saveName + "." + mediaFileData.suffix;
    Tools::writerFile(savePath, QByteArray::fromBase64(mediaFileData.data.toLatin1()));

    return "![" + saveName + "]("
           + "gnr://" + noteModel->getShortUuid() + "/" + saveName + "." + mediaFileData.suffix + ")";
}

QString ImportNoteDialog::getMarkdownForAttachmentFileData(NoteModel *noteModel, MediaFileData &mediaFileData)
{
    const QString saveName = Tools::getShortUuid();
    const QString savePath = noteModel->getNoteDir() + QDir::separator() + saveName + "." + mediaFileData.suffix;
    Tools::writerFile(savePath, QByteArray::fromBase64(mediaFileData.data.toLatin1()));

    return "[" + mediaFileData.fileName + "]("
           + "gnr://" + noteModel->getShortUuid() + "/" + saveName + "." + mediaFileData.suffix + ")";
}

QString ImportNoteDialog::decodeHtmlEntities(QString text)
{
    QMap<QString, QString> entityMap;
    entityMap.insert( "AElig;", "Æ" );
    entityMap.insert( "Aacute;", "Á" );
    entityMap.insert( "Acirc;", "Â" );
    entityMap.insert( "Agrave;", "À" );
    entityMap.insert( "Alpha;", "Α" );
    entityMap.insert( "Aring;", "Å" );
    entityMap.insert( "Atilde;", "Ã" );
    entityMap.insert( "Auml;", "Ä" );
    entityMap.insert( "Beta;", "Β" );
    entityMap.insert( "Ccedil;", "Ç" );
    entityMap.insert( "Chi;", "Χ" );
    entityMap.insert( "Dagger;", "‡" );
    entityMap.insert( "Delta;", "Δ" );
    entityMap.insert( "ETH;", "Ð" );
    entityMap.insert( "Eacute;", "É" );
    entityMap.insert( "Ecirc;", "Ê" );
    entityMap.insert( "Egrave;", "È" );
    entityMap.insert( "Epsilon;", "Ε" );
    entityMap.insert( "Eta;", "Η" );
    entityMap.insert( "Euml;", "Ë" );
    entityMap.insert( "Gamma;", "Γ" );
    entityMap.insert( "Iacute;", "Í" );
    entityMap.insert( "Icirc;", "Î" );
    entityMap.insert( "Igrave;", "Ì" );
    entityMap.insert( "Iota;", "Ι" );
    entityMap.insert( "Iuml;", "Ï" );
    entityMap.insert( "Kappa;", "Κ" );
    entityMap.insert( "Lambda;", "Λ" );
    entityMap.insert( "Mu;", "Μ" );
    entityMap.insert( "Ntilde;", "Ñ" );
    entityMap.insert( "Nu;", "Ν" );
    entityMap.insert( "OElig;", "Œ" );
    entityMap.insert( "Oacute;", "Ó" );
    entityMap.insert( "Ocirc;", "Ô" );
    entityMap.insert( "Ograve;", "Ò" );
    entityMap.insert( "Omega;", "Ω" );
    entityMap.insert( "Omicron;", "Ο" );
    entityMap.insert( "Oslash;", "Ø" );
    entityMap.insert( "Otilde;", "Õ" );
    entityMap.insert( "Ouml;", "Ö" );
    entityMap.insert( "Phi;", "Φ" );
    entityMap.insert( "Pi;", "Π" );
    entityMap.insert( "Prime;", "″" );
    entityMap.insert( "Psi;", "Ψ" );
    entityMap.insert( "Rho;", "Ρ" );
    entityMap.insert( "Scaron;", "Š" );
    entityMap.insert( "Sigma;", "Σ" );
    entityMap.insert( "THORN;", "Þ" );
    entityMap.insert( "Tau;", "Τ" );
    entityMap.insert( "Theta;", "Θ" );
    entityMap.insert( "Uacute;", "Ú" );
    entityMap.insert( "Ucirc;", "Û" );
    entityMap.insert( "Ugrave;", "Ù" );
    entityMap.insert( "Upsilon;", "Υ" );
    entityMap.insert( "Uuml;", "Ü" );
    entityMap.insert( "Xi;", "Ξ" );
    entityMap.insert( "Yacute;", "Ý" );
    entityMap.insert( "Yuml;", "Ÿ" );
    entityMap.insert( "Zeta;", "Ζ" );
    entityMap.insert( "aacute;", "á" );
    entityMap.insert( "acirc;", "â" );
    entityMap.insert( "acute;", "´" );
    entityMap.insert( "aelig;", "æ" );
    entityMap.insert( "agrave;", "à" );
    entityMap.insert( "alefsym;", "ℵ" );
    entityMap.insert( "alpha;", "α" );
    entityMap.insert( "amp;", "&" );
    entityMap.insert( "lt;", "<" );
    entityMap.insert( "gt;", ">" );
    entityMap.insert( "and;", "∧" );
    entityMap.insert( "ang;", "∠" );
    entityMap.insert( "apos;", "'" );
    entityMap.insert( "aring;", "å" );
    entityMap.insert( "asymp;", "≈" );
    entityMap.insert( "atilde;", "ã" );
    entityMap.insert( "auml;", "ä" );
    entityMap.insert( "bdquo;", "„" );
    entityMap.insert( "beta;", "β" );
    entityMap.insert( "brvbar;", "¦" );
    entityMap.insert( "bull;", "•" );
    entityMap.insert( "cap;", "∩" );
    entityMap.insert( "ccedil;", "ç" );
    entityMap.insert( "cedil;", "¸" );
    entityMap.insert( "cent;", "¢" );
    entityMap.insert( "chi;", "χ" );
    entityMap.insert( "circ;", "ˆ" );
    entityMap.insert( "clubs;", "♣" );
    entityMap.insert( "cong;", "≅" );
    entityMap.insert( "copy;", "©" );
    entityMap.insert( "crarr;", "↵" );
    entityMap.insert( "cup;", "∪" );
    entityMap.insert( "curren;", "¤" );
    entityMap.insert( "dArr;", "⇓" );
    entityMap.insert( "dagger;", "†" );
    entityMap.insert( "darr;", "↓" );
    entityMap.insert( "deg;", "°" );
    entityMap.insert( "delta;", "δ" );
    entityMap.insert( "diams;", "♦" );
    entityMap.insert( "divide;", "÷" );
    entityMap.insert( "eacute;", "é" );
    entityMap.insert( "ecirc;", "ê" );
    entityMap.insert( "egrave;", "è" );
    entityMap.insert( "empty;", "∅" );
    entityMap.insert( "emsp;", " " );
    entityMap.insert( "ensp;", " " );
    entityMap.insert( "epsilon;", "ε" );
    entityMap.insert( "equiv;", "≡" );
    entityMap.insert( "eta;", "η" );
    entityMap.insert( "eth;", "ð" );
    entityMap.insert( "euml;", "ë" );
    entityMap.insert( "euro;", "€" );
    entityMap.insert( "exist;", "∃" );
    entityMap.insert( "fnof;", "ƒ" );
    entityMap.insert( "forall;", "∀" );
    entityMap.insert( "frac12;", "½" );
    entityMap.insert( "frac14;", "¼" );
    entityMap.insert( "frac34;", "¾" );
    entityMap.insert( "frasl;", "⁄" );
    entityMap.insert( "gamma;", "γ" );
    entityMap.insert( "ge;", "≥" );
    entityMap.insert( "gt;", ">" );
    entityMap.insert( "hArr;", "⇔" );
    entityMap.insert( "harr;", "↔" );
    entityMap.insert( "hearts;", "♥" );
    entityMap.insert( "hellip;", "…" );
    entityMap.insert( "iacute;", "í" );
    entityMap.insert( "icirc;", "î" );
    entityMap.insert( "iexcl;", "¡" );
    entityMap.insert( "igrave;", "ì" );
    entityMap.insert( "image;", "ℑ" );
    entityMap.insert( "infin;", "∞" );
    entityMap.insert( "int;", "∫" );
    entityMap.insert( "iota;", "ι" );
    entityMap.insert( "iquest;", "¿" );
    entityMap.insert( "isin;", "∈" );
    entityMap.insert( "iuml;", "ï" );
    entityMap.insert( "kappa;", "κ" );
    entityMap.insert( "lArr;", "⇐" );
    entityMap.insert( "lambda;", "λ" );
    entityMap.insert( "lang;", "〈" );
    entityMap.insert( "laquo;", "«" );
    entityMap.insert( "larr;", "←" );
    entityMap.insert( "lceil;", "⌈" );
    entityMap.insert( "ldquo;", "“" );
    entityMap.insert( "le;", "≤" );
    entityMap.insert( "lfloor;", "⌊" );
    entityMap.insert( "lowast;", "∗" );
    entityMap.insert( "loz;", "◊" );
    entityMap.insert( "lrm;", "\xE2\x80\x8E" );
    entityMap.insert( "lsaquo;", "‹" );
    entityMap.insert( "lsquo;", "‘" );
    entityMap.insert( "lt;", "<" );
    entityMap.insert( "macr;", "¯" );
    entityMap.insert( "mdash;", "—" );
    entityMap.insert( "micro;", "µ" );
    entityMap.insert( "middot;", "·" );
    entityMap.insert( "minus;", "−" );
    entityMap.insert( "mu;", "μ" );
    entityMap.insert( "nabla;", "∇" );
    entityMap.insert( "nbsp;", " " );
    entityMap.insert( "ndash;", "–" );
    entityMap.insert( "ne;", "≠" );
    entityMap.insert( "ni;", "∋" );
    entityMap.insert( "not;", "¬" );
    entityMap.insert( "notin;", "∉" );
    entityMap.insert( "nsub;", "⊄" );
    entityMap.insert( "ntilde;", "ñ" );
    entityMap.insert( "nu;", "ν" );
    entityMap.insert( "oacute;", "ó" );
    entityMap.insert( "ocirc;", "ô" );
    entityMap.insert( "oelig;", "œ" );
    entityMap.insert( "ograve;", "ò" );
    entityMap.insert( "oline;", "‾" );
    entityMap.insert( "omega;", "ω" );
    entityMap.insert( "omicron;", "ο" );
    entityMap.insert( "oplus;", "⊕" );
    entityMap.insert( "or;", "∨" );
    entityMap.insert( "ordf;", "ª" );
    entityMap.insert( "ordm;", "º" );
    entityMap.insert( "oslash;", "ø" );
    entityMap.insert( "otilde;", "õ" );
    entityMap.insert( "otimes;", "⊗" );
    entityMap.insert( "ouml;", "ö" );
    entityMap.insert( "para;", "¶" );
    entityMap.insert( "part;", "∂" );
    entityMap.insert( "permil;", "‰" );
    entityMap.insert( "perp;", "⊥" );
    entityMap.insert( "phi;", "φ" );
    entityMap.insert( "pi;", "π" );
    entityMap.insert( "piv;", "ϖ" );
    entityMap.insert( "plusmn;", "±" );
    entityMap.insert( "pound;", "£" );
    entityMap.insert( "prime;", "′" );
    entityMap.insert( "prod;", "∏" );
    entityMap.insert( "prop;", "∝" );
    entityMap.insert( "psi;", "ψ" );
    entityMap.insert( "quot;", "\"" );
    entityMap.insert( "rArr;", "⇒" );
    entityMap.insert( "radic;", "√" );
    entityMap.insert( "rang;", "〉" );
    entityMap.insert( "raquo;", "»" );
    entityMap.insert( "rarr;", "→" );
    entityMap.insert( "rceil;", "⌉" );
    entityMap.insert( "rdquo;", "”" );
    entityMap.insert( "real;", "ℜ" );
    entityMap.insert( "reg;", "®" );
    entityMap.insert( "rfloor;", "⌋" );
    entityMap.insert( "rho;", "ρ" );
    entityMap.insert( "rlm;", "\xE2\x80\x8F" );
    entityMap.insert( "rsaquo;", "›" );
    entityMap.insert( "rsquo;", "’" );
    entityMap.insert( "sbquo;", "‚" );
    entityMap.insert( "scaron;", "š" );
    entityMap.insert( "sdot;", "⋅" );
    entityMap.insert( "sect;", "§" );
    entityMap.insert( "shy;", "\xC2\xAD" );
    entityMap.insert( "sigma;", "σ" );
    entityMap.insert( "sigmaf;", "ς" );
    entityMap.insert( "sim;", "∼" );
    entityMap.insert( "spades;", "♠" );
    entityMap.insert( "sub;", "⊂" );
    entityMap.insert( "sube;", "⊆" );
    entityMap.insert( "sum;", "∑" );
    entityMap.insert( "sup1;", "¹" );
    entityMap.insert( "sup2;", "²" );
    entityMap.insert( "sup3;", "³" );
    entityMap.insert( "sup;", "⊃" );
    entityMap.insert( "supe;", "⊇" );
    entityMap.insert( "szlig;", "ß" );
    entityMap.insert( "tau;", "τ" );
    entityMap.insert( "there4;", "∴" );
    entityMap.insert( "theta;", "θ" );
    entityMap.insert( "thetasym;", "ϑ" );
    entityMap.insert( "thinsp;", " " );
    entityMap.insert( "thorn;", "þ" );
    entityMap.insert( "tilde;", "˜" );
    entityMap.insert( "times;", "×" );
    entityMap.insert( "trade;", "™" );
    entityMap.insert( "uArr;", "⇑" );
    entityMap.insert( "uacute;", "ú" );
    entityMap.insert( "uarr;", "↑" );
    entityMap.insert( "ucirc;", "û" );
    entityMap.insert( "ugrave;", "ù" );
    entityMap.insert( "uml;", "¨" );
    entityMap.insert( "upsih;", "ϒ" );
    entityMap.insert( "upsilon;", "υ" );
    entityMap.insert( "uuml;", "ü" );
    entityMap.insert( "weierp;", "℘" );
    entityMap.insert( "xi;", "ξ" );
    entityMap.insert( "yacute;", "ý" );
    entityMap.insert( "yen;", "¥" );
    entityMap.insert( "yuml;", "ÿ" );
    entityMap.insert( "zeta;", "ζ" );
    entityMap.insert( "zwj;", "\xE2\x80\x8D" );
    entityMap.insert( "zwnj;", "\xE2\x80\x8C" );

    QMapIterator<QString, QString> i( entityMap );

    while (i.hasNext()) {
        i.next();
        text.replace( "&" + i.key(), i.value() );
    }

    return text;
}
