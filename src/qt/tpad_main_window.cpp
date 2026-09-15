#include "tpad_main_window.h"

#include "code_editor.h"
#include "recent_files.h"
#include "spell_checker.h"
#include "syntax_highlighter.h"
#include "tpad_application.h"

#if defined(Q_OS_MACOS)
#include "writing_tools_mac.h"
#endif

#include <QAction>
#include <QCheckBox>
#include <QCloseEvent>
#include <QClipboard>
#include <QCryptographicHash>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QFontDialog>
#include <QFormLayout>
#include <QGuiApplication>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QLockFile>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSettings>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QStatusBar>
#include <QStringConverter>
#include <QTableWidget>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

#include <QPrintDialog>
#include <QPrinter>

#include <algorithm>
#include <limits>
#include <vector>

namespace {

constexpr int kDefaultWidth = 800;
constexpr int kDefaultHeight = 900;

QString selectedTextWithNewlines(const QTextCursor &cursor)
{
    QString text = cursor.selectedText();
    text.replace(QChar::ParagraphSeparator, QLatin1Char('\n'));
    return text;
}

// GTK's COVT option uses g_strcompress(), including its octal and
// unknown-escape behavior.  Decode UTF-8 bytes here for matching parity.
QString decodeCovtText(const QString &text)
{
    const QByteArray source = text.toUtf8();
    QByteArray decoded;
    decoded.reserve(source.size());

    for (qsizetype index = 0; index < source.size(); ++index) {
        const char current = source.at(index);
        if (current != '\\') {
            decoded.append(current);
            continue;
        }

        ++index;
        if (index >= source.size())
            break;

        const char escaped = source.at(index);
        switch (escaped) {
        case 'b':
            decoded.append('\b');
            break;
        case 'f':
            decoded.append('\f');
            break;
        case 'n':
            decoded.append('\n');
            break;
        case 'r':
            decoded.append('\r');
            break;
        case 't':
            decoded.append('\t');
            break;
        case 'v':
            decoded.append('\v');
            break;
        case '\\':
        case '"':
            decoded.append(escaped);
            break;
        default:
            if (escaped >= '0' && escaped <= '7') {
                unsigned int value = static_cast<unsigned int>(escaped - '0');
                int digits = 1;
                while (digits < 3 && index + 1 < source.size()) {
                    const char next = source.at(index + 1);
                    if (next < '0' || next > '7')
                        break;
                    value = value * 8U + static_cast<unsigned int>(next - '0');
                    ++index;
                    ++digits;
                }
                decoded.append(static_cast<char>(value & 0xffU));
            } else {
                decoded.append(escaped);
            }
            break;
        }
    }

    return QString::fromUtf8(decoded);
}

QString encodeCovtText(const QString &text)
{
    const QByteArray source = text.toUtf8();
    QByteArray encoded;
    encoded.reserve(source.size());

    for (const char byte : source) {
        const unsigned char value = static_cast<unsigned char>(byte);
        switch (value) {
        case '\b':
            encoded.append("\\b");
            break;
        case '\f':
            encoded.append("\\f");
            break;
        case '\n':
            encoded.append("\\n");
            break;
        case '\r':
            encoded.append("\\r");
            break;
        case '\t':
            encoded.append("\\t");
            break;
        case '\v':
            encoded.append("\\v");
            break;
        case '\\':
            encoded.append("\\\\");
            break;
        case '"':
            encoded.append("\\\"");
            break;
        default:
            if (value >= 0x20U && value <= 0x7eU) {
                encoded.append(static_cast<char>(value));
            } else {
                encoded.append('\\');
                encoded.append(static_cast<char>('0' + ((value >> 6U) & 0x07U)));
                encoded.append(static_cast<char>('0' + ((value >> 3U) & 0x07U)));
                encoded.append(static_cast<char>('0' + (value & 0x07U)));
            }
            break;
        }
    }
    return QString::fromLatin1(encoded);
}

QAction *makeAction(QObject *parent, const QString &text,
                    const QKeySequence &shortcut = {})
{
    auto *action = new QAction(text, parent);
    if (!shortcut.isEmpty())
        action->setShortcut(shortcut);
    return action;
}

void setActionChecked(QAction *action, bool checked)
{
    const QSignalBlocker blocker(action);
    action->setChecked(checked);
}

QCryptographicHash::Algorithm hashAlgorithm(int value)
{
    return static_cast<QCryptographicHash::Algorithm>(value);
}

QString hashName(QCryptographicHash::Algorithm algorithm)
{
    switch (algorithm) {
    case QCryptographicHash::Md5:
        return QStringLiteral("MD5");
    case QCryptographicHash::Sha256:
        return QStringLiteral("SHA-256");
    case QCryptographicHash::Sha512:
        return QStringLiteral("SHA-512");
    default:
        return TpadMainWindow::tr("Hash");
    }
}

bool readFileBytes(const QString &fileName, QByteArray *bytes, QString *error)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error != nullptr)
            *error = file.errorString();
        return false;
    }
    *bytes = file.readAll();
    if (file.error() != QFileDevice::NoError) {
        if (error != nullptr)
            *error = file.errorString();
        return false;
    }
    return true;
}

} // namespace

TpadMainWindow::TpadMainWindow(QWidget *parent)
    : QMainWindow(parent),
      editor_(new CodeEditor(this)),
      spellChecker_(SpellChecker::create()),
      highlighter_(new TpadSyntaxHighlighter(editor_->document(),
                                             spellChecker_.get())),
      watcher_(new QFileSystemWatcher(this)),
      watchTimer_(new QTimer(this)),
      cursorLabel_(new QLabel(this))
{
    setCentralWidget(editor_);
    setAcceptDrops(true);
    setWindowIcon(QIcon(QStringLiteral(":/icons/tpad-64.png")));
    editor_->setContextMenuPolicy(Qt::CustomContextMenu);
    editor_->setAccessibleName(tr("Text editor"));

    watchTimer_->setSingleShot(true);
    watchTimer_->setInterval(250);

    createActions();
    createMenus();
    createToolBar();
    statusBar()->addPermanentWidget(cursorLabel_);

    connect(editor_->document(), &QTextDocument::modificationChanged,
            this, [this] { updateWindowTitle(); });
    connect(editor_->document(), &QTextDocument::undoAvailable,
            undoAction_, &QAction::setEnabled);
    connect(editor_->document(), &QTextDocument::redoAvailable,
            redoAction_, &QAction::setEnabled);
    connect(editor_, &QPlainTextEdit::copyAvailable, this,
            [this](bool available) {
                cutAction_->setEnabled(available);
                copyAction_->setEnabled(available);
            });
    connect(editor_, &QPlainTextEdit::cursorPositionChanged,
            this, [this] { updateCursorStatus(); });
    connect(editor_, &QWidget::customContextMenuRequested,
            this, [this](const QPoint &point) { showEditorContextMenu(point); });
    connect(watcher_, &QFileSystemWatcher::fileChanged,
            this, [this] { queueExternalChangeCheck(); });
    connect(watchTimer_, &QTimer::timeout,
            this, [this] { processExternalChange(); });

    readSettings();
    editor_->document()->setModified(false);
    updateWindowTitle();
    updateCursorStatus();
    updateActionState();

#if defined(Q_OS_MACOS)
    setUnifiedTitleAndToolBarOnMac(true);
#elif defined(Q_OS_WIN)
    // Explicitly set the application icon so unpackaged Windows builds also
    // receive it before a .ico resource is added by the deployment project.
    setWindowIcon(QIcon(QStringLiteral(":/icons/tpad-64.png")));
#endif
}

TpadMainWindow::~TpadMainWindow() = default;

void TpadMainWindow::createActions()
{
    saveAction_ = makeAction(this, tr("&Save"), QKeySequence::Save);
    reloadAction_ = makeAction(this, tr("&Reload"), QKeySequence::Refresh);
    copyPathAction_ = makeAction(this, tr("Copy File &Path"));
    undoAction_ = makeAction(this, tr("&Undo"), QKeySequence::Undo);
    redoAction_ = makeAction(this, tr("&Redo"), QKeySequence::Redo);
    cutAction_ = makeAction(this, tr("Cu&t"), QKeySequence::Cut);
    copyAction_ = makeAction(this, tr("&Copy"), QKeySequence::Copy);

    wrapAction_ = makeAction(this, tr("Word &Wrap"));
    lineNumbersAction_ = makeAction(this, tr("Show Line &Numbers"));
    syntaxAction_ = makeAction(this, tr("Syntax &Highlighting"));
    spellAction_ = makeAction(this, tr("&Spell Checking"));
    bomAction_ = makeAction(this, tr("Write UTF-8 &BOM"));
    fullPathAction_ = makeAction(this, tr("Show &Full File Path"));
    openGuardAction_ = makeAction(this, tr("Guard Against Concurrent Editing"));
    stayOnTopAction_ = makeAction(this, tr("Keep Window on &Top"));

    for (QAction *action : {wrapAction_, lineNumbersAction_, syntaxAction_,
                            spellAction_, bomAction_, fullPathAction_, openGuardAction_,
                            stayOnTopAction_})
        action->setCheckable(true);

    spellAction_->setEnabled(spellChecker_ != nullptr
                             && spellChecker_->isAvailable());
    if (spellAction_->isEnabled()) {
        const QString language = spellChecker_->language();
        if (!language.isEmpty())
            spellAction_->setToolTip(tr("System spelling service (%1)").arg(language));
    } else {
        spellAction_->setToolTip(tr("No system spelling service is available."));
    }

    connect(saveAction_, &QAction::triggered, this,
            [this] { (void) saveFile(); });
    connect(reloadAction_, &QAction::triggered, this,
            [this] { (void) reloadFile(); });
    connect(copyPathAction_, &QAction::triggered, this,
            [this] { copyCurrentPath(); });
    connect(undoAction_, &QAction::triggered, editor_, &QPlainTextEdit::undo);
    connect(redoAction_, &QAction::triggered, editor_, &QPlainTextEdit::redo);
    connect(cutAction_, &QAction::triggered, editor_, &QPlainTextEdit::cut);
    connect(copyAction_, &QAction::triggered, editor_, &QPlainTextEdit::copy);
    connect(wrapAction_, &QAction::toggled,
            this, [this](bool checked) { setWrapEnabled(checked); });
    connect(lineNumbersAction_, &QAction::toggled,
            this, [this](bool checked) { setLineNumbersEnabled(checked); });
    connect(syntaxAction_, &QAction::toggled,
            this, [this](bool checked) { setSyntaxEnabled(checked); });
    connect(spellAction_, &QAction::toggled,
            this, [this](bool checked) { setSpellCheckingEnabled(checked); });
    connect(bomAction_, &QAction::toggled, this, [this](bool checked) {
        writeUtf8Bom_ = checked;
        editor_->document()->setModified(true);
    });
    connect(fullPathAction_, &QAction::toggled,
            this, [this](bool checked) { setFullPathEnabled(checked); });
    connect(openGuardAction_, &QAction::toggled,
            this, [this](bool checked) {
                setOpenGuardEnabled(checked);
                // This is an application-wide preference.  Save the effective
                // state immediately so a new window does not keep using the
                // old value until every existing window has been closed.
                QSettings settings;
                settings.setValue(QStringLiteral("files/openGuard"),
                                  openGuardAction_->isChecked());
            });
    connect(stayOnTopAction_, &QAction::toggled,
            this, [this](bool checked) { setStayOnTop(checked); });

    undoAction_->setEnabled(false);
    redoAction_->setEnabled(false);
    cutAction_->setEnabled(false);
    copyAction_->setEnabled(false);
}

void TpadMainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *newAction = makeAction(this, tr("&New Window"), QKeySequence::New);
    QAction *openAction = makeAction(this, tr("&Open…"), QKeySequence::Open);
    QAction *saveAsAction = makeAction(this, tr("Save &As…"), QKeySequence::SaveAs);
    QAction *printAction = makeAction(this, tr("&Print…"), QKeySequence::Print);
    QAction *closeAction = makeAction(this, tr("&Close Window"), QKeySequence::Close);
    QAction *quitAction = makeAction(this, tr("&Quit"), QKeySequence::Quit);
    recentFilesMenu_ = new QMenu(tr("Open &Recent"), fileMenu);
    newAction->setObjectName(QStringLiteral("newAction"));
    openAction->setObjectName(QStringLiteral("openAction"));
    saveAsAction->setObjectName(QStringLiteral("saveAsAction"));
    printAction->setObjectName(QStringLiteral("printAction"));
    quitAction->setObjectName(QStringLiteral("quitAction"));
    newAction->setIconText(tr("New"));
    openAction->setIconText(tr("Open"));
    saveAsAction->setIconText(tr("Save As"));
    quitAction->setMenuRole(QAction::QuitRole);

    connect(newAction, &QAction::triggered, this, [this] { newWindow(); });
    connect(openAction, &QAction::triggered, this, [this] { openFiles(); });
    connect(fileMenu, &QMenu::aboutToShow,
            this, [this] { updateRecentFilesMenu(); });
    connect(saveAsAction, &QAction::triggered, this,
            [this] { (void) saveFileAs(); });
    connect(printAction, &QAction::triggered, this, [this] { printDocument(); });
    connect(closeAction, &QAction::triggered, this, &QWidget::close);
    connect(quitAction, &QAction::triggered, this, [] {
        if (TpadApplication *application = TpadApplication::instance())
            application->requestQuit();
    });

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addMenu(recentFilesMenu_);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction_);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(reloadAction_);
    fileMenu->addSeparator();
    fileMenu->addAction(printAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    fileMenu->addAction(quitAction);

    QSettings settings;
    recentFilesMenu_->setEnabled(
        settings.value(QStringLiteral("files/recentEnabled"), true).toBool());

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QAction *pasteAction = makeAction(this, tr("&Paste"), QKeySequence::Paste);
    QAction *selectAllAction = makeAction(this, tr("Select &All"), QKeySequence::SelectAll);
    QAction *findAction = makeAction(this, tr("&Find…"), QKeySequence::Find);
    QAction *replaceAction = makeAction(this, tr("Find and &Replace…"),
                                        QKeySequence(Qt::CTRL | Qt::Key_H));
    connect(pasteAction, &QAction::triggered, editor_, &QPlainTextEdit::paste);
    connect(selectAllAction, &QAction::triggered, editor_, &QPlainTextEdit::selectAll);
    connect(findAction, &QAction::triggered, this,
            [this] { showFindDialog(); });
    connect(replaceAction, &QAction::triggered, this,
            [this] { showFindDialog(); });

    editMenu->addAction(undoAction_);
    editMenu->addAction(redoAction_);
    editMenu->addSeparator();
    editMenu->addAction(cutAction_);
    editMenu->addAction(copyAction_);
    editMenu->addAction(pasteAction);
    editMenu->addAction(selectAllAction);
    editMenu->addSeparator();
    editMenu->addAction(findAction);
    editMenu->addAction(replaceAction);
    editMenu->addAction(copyPathAction_);

    QMenu *transformMenu = editMenu->addMenu(tr("&Transform"));
    QAction *upperAction = transformMenu->addAction(tr("Make &Uppercase"));
    QAction *lowerAction = transformMenu->addAction(tr("Make &Lowercase"));
    QAction *reverseAction = transformMenu->addAction(tr("&Reverse Text"));
    upperAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_U));
    lowerAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    reverseAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    connect(upperAction, &QAction::triggered, this,
            [this] { transformSelection(1); });
    connect(lowerAction, &QAction::triggered, this,
            [this] { transformSelection(2); });
    connect(reverseAction, &QAction::triggered, this,
            [this] { transformSelection(3); });

    editMenu->addSeparator();
    QAction *statsAction = editMenu->addAction(tr("Text &Statistics…"));
    QAction *frequencyAction = editMenu->addAction(tr("&Frequency Analysis…"));
    connect(statsAction, &QAction::triggered, this,
            [this] { showTextStatistics(); });
    connect(frequencyAction, &QAction::triggered, this,
            [this] { showFrequencyAnalysis(); });

    QMenu *hashMenu = menuBar()->addMenu(tr("&Hash / Base64"));
    QMenu *selectionHash = hashMenu->addMenu(tr("Hash Selection"));
    QMenu *fileHashMenu = hashMenu->addMenu(tr("Hash File"));
    const QList<QPair<QString, QCryptographicHash::Algorithm>> hashes = {
        {QStringLiteral("SHA-512"), QCryptographicHash::Sha512},
        {QStringLiteral("SHA-256"), QCryptographicHash::Sha256},
        {QStringLiteral("MD5"), QCryptographicHash::Md5}
    };
    for (const auto &entry : hashes) {
        QAction *selectionAction = selectionHash->addAction(entry.first);
        QAction *fileAction = fileHashMenu->addAction(entry.first);
        const int algorithm = static_cast<int>(entry.second);
        connect(selectionAction, &QAction::triggered, this,
                [this, algorithm] { copyDigestToClipboard(algorithm); });
        connect(fileAction, &QAction::triggered, this,
                [this, algorithm] { copyFileDigestToClipboard(algorithm); });
    }
    hashMenu->addSeparator();
    QAction *encodeSelection = hashMenu->addAction(tr("Base64 Encode Selection"));
    QAction *decodeSelection = hashMenu->addAction(tr("Base64 Decode Selection"));
    QAction *encodeFile = hashMenu->addAction(tr("Base64 Encode File…"));
    QAction *decodeFile = hashMenu->addAction(tr("Base64 Decode File…"));
    connect(encodeSelection, &QAction::triggered, this,
            [this] { base64Selection(false); });
    connect(decodeSelection, &QAction::triggered, this,
            [this] { base64Selection(true); });
    connect(encodeFile, &QAction::triggered, this,
            [this] { base64File(false); });
    connect(decodeFile, &QAction::triggered, this,
            [this] { base64File(true); });

    QMenu *settingsMenu = menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(wrapAction_);
    settingsMenu->addAction(lineNumbersAction_);
    settingsMenu->addAction(syntaxAction_);
    settingsMenu->addAction(spellAction_);
    settingsMenu->addAction(bomAction_);
    settingsMenu->addAction(fullPathAction_);
    settingsMenu->addAction(openGuardAction_);
    settingsMenu->addAction(stayOnTopAction_);
    settingsMenu->addSeparator();
    QAction *fontAction = settingsMenu->addAction(tr("Select &Font…"));
    QAction *preferencesAction = settingsMenu->addAction(tr("&Preferences…"));
    preferencesAction->setMenuRole(QAction::PreferencesRole);
    connect(fontAction, &QAction::triggered, this, [this] { chooseFont(); });
    connect(preferencesAction, &QAction::triggered,
            this, [this] { showPreferences(); });

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAction = helpMenu->addAction(tr("&About Tpad"));
    aboutAction->setMenuRole(QAction::AboutRole);
    connect(aboutAction, &QAction::triggered, this, [this] { showAbout(); });
}

void TpadMainWindow::createToolBar()
{
    QToolBar *bar = addToolBar(tr("File and edit"));
    bar->setObjectName(QStringLiteral("mainToolBar"));
    bar->setMovable(false);
    bar->setFloatable(false);
    bar->setToolButtonStyle(Qt::ToolButtonTextOnly);

    QAction *newAction = findChild<QAction *>(QStringLiteral("newAction"));
    QAction *openAction = findChild<QAction *>(QStringLiteral("openAction"));
    QAction *saveAsAction = findChild<QAction *>(QStringLiteral("saveAsAction"));
    QAction *quitAction = findChild<QAction *>(QStringLiteral("quitAction"));
    if (newAction != nullptr)
        bar->addAction(newAction);
    if (openAction != nullptr)
        bar->addAction(openAction);
    bar->addAction(saveAction_);
    if (saveAsAction != nullptr)
        bar->addAction(saveAsAction);
    bar->addSeparator();
    bar->addAction(reloadAction_);
    bar->addSeparator();
    bar->addAction(undoAction_);
    bar->addAction(redoAction_);
    bar->addSeparator();
    if (quitAction != nullptr)
        bar->addAction(quitAction);
}

void TpadMainWindow::readSettings()
{
    QSettings settings;
    const int width = settings.value(QStringLiteral("window/defaultWidth"),
                                     kDefaultWidth).toInt();
    const int height = settings.value(QStringLiteral("window/defaultHeight"),
                                      kDefaultHeight).toInt();
    resize(qBound(320, width, 6000), qBound(240, height, 6000));

    // Apply each preference through the same setter used at runtime.  A new
    // QAction starts unchecked, so setChecked(false) does not emit toggled();
    // relying on that signal leaves default-on editor features out of sync
    // with their checked state.
    setWrapEnabled(
        settings.value(QStringLiteral("editor/wrap"), true).toBool());
    setLineNumbersEnabled(
        settings.value(QStringLiteral("editor/lineNumbers"), true).toBool());
    setSyntaxEnabled(
        settings.value(QStringLiteral("editor/syntax"), true).toBool());
    setFullPathEnabled(
        settings.value(QStringLiteral("window/fullPath"), true).toBool());
    setOpenGuardEnabled(
        settings.value(QStringLiteral("files/openGuard"), false).toBool());
    setSpellCheckingEnabled(
        settings.value(QStringLiteral("editor/spelling"), false).toBool());
    setActionChecked(stayOnTopAction_, false);
    setActionChecked(bomAction_, false);

    QFont font = editor_->font();
    const QString serializedFont = settings.value(
        QStringLiteral("editor/font")).toString();
    if (!serializedFont.isEmpty() && font.fromString(serializedFont)) {
        editor_->setFont(font);
        editor_->document()->setDefaultFont(font);
    }
    editor_->setTabWidth(qBound(1,
        settings.value(QStringLiteral("editor/tabWidth"), 4).toInt(), 16));
}

void TpadMainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue(QStringLiteral("editor/wrap"), wrapAction_->isChecked());
    settings.setValue(QStringLiteral("editor/lineNumbers"),
                      lineNumbersAction_->isChecked());
    settings.setValue(QStringLiteral("editor/syntax"), syntaxAction_->isChecked());
    settings.setValue(QStringLiteral("editor/spelling"), spellAction_->isChecked());
    settings.setValue(QStringLiteral("editor/font"), editor_->font().toString());
    settings.setValue(QStringLiteral("window/fullPath"),
                      fullPathAction_->isChecked());
}

void TpadMainWindow::updateRecentFilesMenu()
{
    QSettings settings;
    const bool enabled = settings.value(
        QStringLiteral("files/recentEnabled"), true).toBool();
    recentFilesMenu_->clear();
    recentFilesMenu_->setEnabled(enabled);
    if (!enabled)
        return;

    QString error;
    const QStringList files = TpadRecentFiles::load(&error);
    if (!error.isEmpty())
        qWarning().noquote() << tr("Unable to load recent files: %1").arg(error);
    if (files.isEmpty()) {
        QAction *emptyAction = recentFilesMenu_->addAction(tr("No Recent Files"));
        emptyAction->setEnabled(false);
        return;
    }
    for (const QString &path : files) {
        QString label = QDir::toNativeSeparators(path);
        label.replace(QLatin1Char('&'), QStringLiteral("&&"));
        label.replace(QLatin1Char('\r'), QLatin1Char(' '));
        label.replace(QLatin1Char('\n'), QLatin1Char(' '));
        label.replace(QLatin1Char('\t'), QLatin1Char(' '));
        QAction *action = recentFilesMenu_->addAction(label);
        action->setToolTip(QDir::toNativeSeparators(path));
        connect(action, &QAction::triggered, this, [this, path] {
            if (TpadApplication *application = TpadApplication::instance())
                application->openFileFromWindow(path, this);
            else if (canAdoptFile())
                (void) loadFile(path);
        });
    }
}

void TpadMainWindow::recordRecentFile(const QString &fileName)
{
    QSettings settings;
    if (!settings.value(QStringLiteral("files/recentEnabled"), true).toBool())
        return;
    const int maximum = qBound(
        0, settings.value(QStringLiteral("files/recentMax"),
                          TpadRecentFiles::DefaultLimit).toInt(),
        TpadRecentFiles::MaximumLimit);
    QString error;
    if (!TpadRecentFiles::add(fileName, maximum, &error))
        qWarning().noquote() << tr("Unable to update recent files: %1").arg(error);
}

bool TpadMainWindow::loadFile(const QString &fileName, bool createIfMissing)
{
    const QString path = normalizedPath(fileName);
    const bool switchingFiles = path != currentFile_;
    const QFileInfo inputInfo(path);
    if (!path.isEmpty()
        && ((inputInfo.exists() && !inputInfo.isFile())
            || (!inputInfo.exists() && inputInfo.isSymLink()))) {
        QMessageBox::critical(this, tr("Unsupported File"),
                              tr("“%1” is not a regular file. Tpad will not "
                                 "read directories, devices, sockets, or pipes.")
                                  .arg(QDir::toNativeSeparators(path)));
        return false;
    }
    bool proceed = true;
    std::unique_ptr<QLockFile> candidateLock;
    if (!path.isEmpty() && openGuardAction_->isChecked()
        && (path != currentFile_ || documentLock_ == nullptr)) {
        candidateLock = tryDocumentLock(path, &proceed);
        if (!proceed)
            return false;
    }
    // Preserve Tpad's command-line behavior: a named file that does not yet
    // exist becomes an empty document at that path.  QSaveFile makes even
    // this initial creation atomic and leaves no partial file on failure.
    if (createIfMissing && !path.isEmpty() && !QFileInfo::exists(path)) {
        QSaveFile newFile(path);
        newFile.setDirectWriteFallback(false);
        if (!newFile.open(QIODevice::WriteOnly) || !newFile.commit()) {
            QMessageBox::critical(this, tr("Unable to Create File"),
                                  tr("Tpad could not create “%1”.\n\n%2")
                                      .arg(QDir::toNativeSeparators(path),
                                           newFile.errorString()));
            return false;
        }
    }
    QByteArray bytes;
    QString ioError;
    if (path.isEmpty() || !readFileBytes(path, &bytes, &ioError)) {
        QMessageBox::critical(this, tr("Unable to Open File"),
                              tr("Tpad could not read “%1”.\n\n%2")
                                  .arg(QDir::toNativeSeparators(fileName), ioError));
        return false;
    }

    QString text;
    QString decodeError;
    bool hadBom = false;
    bool requiresEncodingConversion = false;
    LineEnding ending = LineEnding::Lf;
    if (!decodeDocument(bytes, &text, &hadBom, &ending,
                        &requiresEncodingConversion, &decodeError)) {
        QMessageBox::critical(this, tr("Unsupported File"),
                              tr("Tpad could not open “%1”.\n\n%2")
                                  .arg(QDir::toNativeSeparators(path), decodeError));
        return false;
    }
    if (requiresEncodingConversion) {
        QMessageBox box(QMessageBox::Information, tr("Convert Text Encoding"),
                        tr("This file is not UTF-8. Tpad can open it using its "
                           "detected or system text encoding, but it will be "
                           "converted to UTF-8 the next time it is saved."),
                        QMessageBox::NoButton, this);
        QPushButton *convert = box.addButton(tr("Open and Convert"),
                                             QMessageBox::AcceptRole);
        box.addButton(tr("Cancel"), QMessageBox::RejectRole);
        box.setDefaultButton(convert);
        box.exec();
        if (box.clickedButton() != convert)
            return false;
    }

    saving_ = true;
    if (!watcher_->files().isEmpty())
        watcher_->removePaths(watcher_->files());
    currentFile_ = path;
    if (switchingFiles || candidateLock != nullptr)
        documentLock_ = std::move(candidateLock);
    lineEnding_ = ending;
    writeUtf8Bom_ = hadBom;
    bomAction_->blockSignals(true);
    bomAction_->setChecked(hadBom);
    bomAction_->blockSignals(false);
    editor_->setPlainText(text);
    highlighter_->setFileName(path);
    cleanDocumentBytes_ = encodeDocument();
    editor_->document()->setModified(false);
    lastDiskHash_ = QCryptographicHash::hash(bytes, QCryptographicHash::Sha512);
    externalChangePending_ = false;
    saving_ = false;
    updateFileWatcher();
    updateWindowTitle();
    updateActionState();
    statusBar()->showMessage(tr("Opened %1").arg(QDir::toNativeSeparators(path)),
                             3000);
    recordRecentFile(path);
    return true;
}

bool TpadMainWindow::canAdoptFile() const
{
    return currentFile_.isEmpty() && !hasUnsavedChanges()
           && editor_->document()->isEmpty();
}

void TpadMainWindow::newWindow()
{
    if (TpadApplication *application = TpadApplication::instance())
        application->createNewWindow();
}

void TpadMainWindow::openFiles()
{
    const QString initial = currentFile_.isEmpty()
                                ? QDir::homePath() : QFileInfo(currentFile_).path();
    const QStringList files = QFileDialog::getOpenFileNames(
        this, tr("Open Files"), initial, tr("All Files (*)"));
    if (files.isEmpty())
        return;

    TpadApplication *application = TpadApplication::instance();
    for (const QString &file : files) {
        if (application != nullptr)
            application->openFileFromWindow(file, this);
        else if (canAdoptFile())
            loadFile(file);
    }
}

bool TpadMainWindow::saveFile()
{
    if (currentFile_.isEmpty())
        return saveFileAs();

    bool allowExternalOverwrite = false;
    if (diskContentChanged()) {
        switch (askExternalChangeChoice(true)) {
        case ExternalChoice::Overwrite:
            allowExternalOverwrite = true;
            break;
        case ExternalChoice::SaveAs:
            return saveFileAs();
        case ExternalChoice::Reload:
            return reloadFile();
        case ExternalChoice::Cancel:
            return false;
        }
    }
    return saveToPath(currentFile_, allowExternalOverwrite);
}

bool TpadMainWindow::saveFileAs()
{
    const QString initial = currentFile_.isEmpty()
        ? QDir::home().filePath(tr("Untitled.txt")) : currentFile_;
    const QString selected = QFileDialog::getSaveFileName(
        this, tr("Save File"), initial, tr("All Files (*)"));
    if (selected.isEmpty())
        return false;
    if (!currentFile_.isEmpty()
        && normalizedPath(selected) == normalizedPath(currentFile_))
        return saveFile();
    return saveToPath(selected);
}

bool TpadMainWindow::saveToPath(const QString &fileName,
                                bool allowExternalOverwrite)
{
    // Resolve an existing symbolic link before giving the destination to
    // QSaveFile. Its atomic rename must update the linked document, not
    // replace the link itself with a new regular file.
    const QString path = normalizedPath(fileName);
    const bool switchingFiles = path != currentFile_;
    const QFileInfo initialInfo(path);
    const bool targetExisted = initialInfo.exists();
    if ((targetExisted && !initialInfo.isFile())
        || (!targetExisted && initialInfo.isSymLink())) {
        QMessageBox::critical(this, tr("Unsafe Save Target"),
                              tr("“%1” is not a regular file. Tpad will not "
                                 "replace directories, devices, sockets, pipes, "
                                 "or dangling symbolic links.")
                                  .arg(QDir::toNativeSeparators(path)));
        return false;
    }

    bool initialHashOk = false;
    const QByteArray initialHash = targetExisted
        ? fileHash(path, &initialHashOk) : QByteArray();
    if (targetExisted && !initialHashOk) {
        QMessageBox::critical(this, tr("Unable to Inspect Save Target"),
                              tr("Tpad could not verify “%1” before saving.")
                                  .arg(QDir::toNativeSeparators(path)));
        return false;
    }
    if (!switchingFiles && !allowExternalOverwrite
        && (!targetExisted || initialHash != lastDiskHash_)) {
        QMessageBox::warning(this, tr("File Changed on Disk"),
                             tr("The file changed before Tpad could begin the "
                                "save. Nothing was overwritten; review the "
                                "external version and try again."));
        externalChangePending_ = true;
        return false;
    }
    bool proceed = true;
    std::unique_ptr<QLockFile> candidateLock;
    if (switchingFiles && openGuardAction_->isChecked()) {
        candidateLock = tryDocumentLock(path, &proceed);
        if (!proceed)
            return false;
    }
    QSaveFile file(path);
    file.setDirectWriteFallback(false);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Unable to Save File"),
                              tr("Tpad could not write “%1”.\n\n%2")
                                  .arg(QDir::toNativeSeparators(path),
                                       file.errorString()));
        return false;
    }

    const QByteArray bytes = encodeDocument();
    saving_ = true;
    if (file.write(bytes) != bytes.size()) {
        const QString error = file.errorString();
        file.cancelWriting();
        saving_ = false;
        QMessageBox::critical(this, tr("Unable to Save File"),
                              tr("Tpad could not write the temporary file for “%1”.\n\n%2")
                                  .arg(QDir::toNativeSeparators(path), error));
        updateFileWatcher();
        return false;
    }

    // QSaveFile keeps the destination intact until commit. Revalidate it at
    // the last possible moment so an editor that writes during a long save is
    // never silently overwritten.
    const QFileInfo finalInfo(path);
    bool finalHashOk = false;
    const QByteArray finalHash = finalInfo.exists()
        ? fileHash(path, &finalHashOk) : QByteArray();
    const bool targetChanged = targetExisted != finalInfo.exists()
        || (finalInfo.exists() && (!finalInfo.isFile() || !finalHashOk
                                   || finalHash != initialHash));
    if (targetChanged) {
        file.cancelWriting();
        saving_ = false;
        externalChangePending_ = true;
        updateFileWatcher();
        QMessageBox::warning(this, tr("File Changed During Save"),
                             tr("Another application changed “%1” while Tpad "
                                "was preparing the save. Nothing was overwritten.")
                                  .arg(QDir::toNativeSeparators(path)));
        return false;
    }
    if (!file.commit()) {
        const QString error = file.errorString();
        saving_ = false;
        QMessageBox::critical(this, tr("Unable to Save File"),
                              tr("Tpad could not safely replace “%1”.\n\n%2")
                                  .arg(QDir::toNativeSeparators(path), error));
        updateFileWatcher();
        return false;
    }

    currentFile_ = normalizedPath(path);
    if (switchingFiles)
        documentLock_ = std::move(candidateLock);
    lastDiskHash_ = QCryptographicHash::hash(bytes, QCryptographicHash::Sha512);
    externalChangePending_ = false;
    highlighter_->setFileName(currentFile_);
    cleanDocumentBytes_ = bytes;
    editor_->document()->setModified(false);
    saving_ = false;
    updateFileWatcher();
    updateWindowTitle();
    updateActionState();
    statusBar()->showMessage(tr("Saved %1")
                                 .arg(QDir::toNativeSeparators(currentFile_)), 3000);
    recordRecentFile(currentFile_);
    return true;
}

bool TpadMainWindow::reloadFile()
{
    if (currentFile_.isEmpty())
        return false;
    if (hasUnsavedChanges()) {
        const QMessageBox::StandardButton response = QMessageBox::question(
            this, tr("Reload File"),
            tr("Reloading will discard the unsaved changes in this window."),
            QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
        if (response != QMessageBox::Discard)
            return false;
    }
    return loadFile(currentFile_, false);
}

bool TpadMainWindow::maybeSave()
{
    if (!hasUnsavedChanges()) {
        // Formatting-only QTextDocument changes must never turn a clean text
        // file into an unsaved document.
        editor_->document()->setModified(false);
        return true;
    }
    QMessageBox box(QMessageBox::Question, tr("Save Changes"),
                    tr("The document has unsaved changes. Save them before closing?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                    this);
    box.setDefaultButton(QMessageBox::Save);
    const int result = box.exec();
    if (result == QMessageBox::Save)
        return saveFile();
    return result == QMessageBox::Discard;
}

void TpadMainWindow::printDocument()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setDocName(currentFile_.isEmpty()
                           ? tr("Untitled") : QFileInfo(currentFile_).fileName());
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Document"));
    if (dialog.exec() == QDialog::Accepted)
        editor_->document()->print(&printer);
}

void TpadMainWindow::closeEvent(QCloseEvent *event)
{
    if (closing_ || maybeSave()) {
        closing_ = true;
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void TpadMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void TpadMainWindow::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasUrls())
        return;
    editor_->insertFromMimeData(event->mimeData());
    event->acceptProposedAction();
}

bool TpadMainWindow::hasUnsavedChanges() const
{
    return encodeDocument() != cleanDocumentBytes_;
}

void TpadMainWindow::updateWindowTitle()
{
    QString label;
    if (currentFile_.isEmpty())
        label = tr("Untitled");
    else if (fullPathAction_ != nullptr && fullPathAction_->isChecked())
        label = QDir::toNativeSeparators(currentFile_);
    else
        label = QFileInfo(currentFile_).fileName();

    setWindowFilePath(currentFile_);
    setWindowTitle(tr("%1[*] — Tpad").arg(label));
    setWindowModified(hasUnsavedChanges());
}

void TpadMainWindow::updateCursorStatus()
{
    const QTextCursor cursor = editor_->textCursor();
    cursorLabel_->setText(tr("Ln %1, Col %2")
                              .arg(cursor.blockNumber() + 1)
                              .arg(cursor.positionInBlock() + 1));
}

void TpadMainWindow::updateActionState()
{
    const bool hasFile = !currentFile_.isEmpty();
    reloadAction_->setEnabled(hasFile);
    copyPathAction_->setEnabled(hasFile);
}

void TpadMainWindow::showFindDialog()
{
    if (findDialog_ != nullptr) {
        findDialog_->show();
        findDialog_->raise();
        findDialog_->activateWindow();
        if (auto *findEdit = findDialog_->findChild<QLineEdit *>(
                QStringLiteral("findTextEdit"))) {
            findEdit->setFocus();
            findEdit->selectAll();
        }
        return;
    }

    auto *dialog = new QDialog(this);
    findDialog_ = dialog;
    dialog->setObjectName(QStringLiteral("findReplaceDialog"));
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(tr("Find / Find & Replace Text"));
    dialog->setModal(false);
    dialog->setSizeGripEnabled(false);
    connect(dialog, &QObject::destroyed, this, [this] { findDialog_ = nullptr; });

    auto *layout = new QHBoxLayout(dialog);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    auto *findLabel = new QLabel(tr("Search Text For:"), dialog);
    auto *findEdit = new QLineEdit(dialog);
    findEdit->setObjectName(QStringLiteral("findTextEdit"));
    findEdit->setMaxLength(65536);
    findEdit->setMinimumWidth(168);
    findLabel->setBuddy(findEdit);
    const QString initial = selectedTextWithNewlines(editor_->textCursor());
    if (!initial.contains(QLatin1Char('\n')) && initial.size() <= 256)
        findEdit->setText(encodeCovtText(initial));

    auto *replaceLabel = new QLabel(tr("Replace Text With:"), dialog);
    auto *replaceEdit = new QLineEdit(dialog);
    replaceEdit->setObjectName(QStringLiteral("replaceTextEdit"));
    replaceEdit->setMaxLength(65536);
    replaceEdit->setMinimumWidth(168);
    replaceLabel->setBuddy(replaceEdit);

    auto *findButton = new QPushButton(tr("Find"), dialog);
    findButton->setObjectName(QStringLiteral("findButton"));
    findButton->setMinimumWidth(95);
    auto *replaceButton = new QPushButton(tr("Replace"), dialog);
    replaceButton->setObjectName(QStringLiteral("replaceButton"));
    replaceButton->setMinimumWidth(95);
    QString replaceAllText = tr("Find / Find & Replace Text");
    replaceAllText.replace(QLatin1Char('&'), QStringLiteral("&&"));
    auto *replaceAllButton = new QPushButton(replaceAllText, dialog);
    replaceAllButton->setObjectName(QStringLiteral("replaceAllButton"));
    replaceAllButton->setMinimumWidth(210);
    auto *covtCheck = new QCheckBox(tr("COVT"), dialog);
    covtCheck->setObjectName(QStringLiteral("covtCheckBox"));
    covtCheck->setChecked(true);
    covtCheck->setMinimumWidth(95);
    auto *caseCheck = new QCheckBox(tr("Match Case"), dialog);
    caseCheck->setObjectName(QStringLiteral("matchCaseCheckBox"));
    caseCheck->setChecked(false);
    caseCheck->setMinimumWidth(95);

    layout->addWidget(findLabel);
    layout->addWidget(findEdit);
    layout->addWidget(replaceLabel);
    layout->addWidget(replaceEdit);
    layout->addWidget(findButton);
    layout->addWidget(replaceButton);
    layout->addSpacing(findButton->minimumWidth());
    layout->addWidget(replaceAllButton);
    layout->addWidget(caseCheck);
    layout->addWidget(covtCheck);

    connect(findButton, &QPushButton::clicked, dialog,
            [this, findEdit, caseCheck, covtCheck] {
        const QString needle = covtCheck->isChecked()
            ? decodeCovtText(findEdit->text()) : findEdit->text();
        findNext(needle, caseCheck->isChecked());
    });
    connect(replaceButton, &QPushButton::clicked, dialog,
            [this, findEdit, replaceEdit, caseCheck, covtCheck] {
        const QString needle = covtCheck->isChecked()
            ? decodeCovtText(findEdit->text()) : findEdit->text();
        const QString replacement = covtCheck->isChecked()
            ? decodeCovtText(replaceEdit->text()) : replaceEdit->text();
        replaceCurrentOrNext(needle, replacement, caseCheck->isChecked());
    });
    connect(replaceAllButton, &QPushButton::clicked, dialog,
            [this, findEdit, replaceEdit, caseCheck, covtCheck] {
        const QString needle = covtCheck->isChecked()
            ? decodeCovtText(findEdit->text()) : findEdit->text();
        const QString replacement = covtCheck->isChecked()
            ? decodeCovtText(replaceEdit->text()) : replaceEdit->text();
        replaceAll(needle, replacement, caseCheck->isChecked());
    });
    connect(findEdit, &QLineEdit::returnPressed, findButton, &QPushButton::click);

    dialog->setFixedSize(dialog->sizeHint());
    findEdit->setFocus();
    findEdit->selectAll();
    dialog->show();
}

bool TpadMainWindow::findNext(const QString &needle, bool caseSensitive,
                              bool backwards)
{
    if (needle.isEmpty())
        return false;
    // QTextDocument::find() cannot match paragraph separators.  Plain-text
    // UTF-16 offsets map directly to QTextCursor positions, including LF.
    const QString text = editor_->toPlainText();
    const Qt::CaseSensitivity sensitivity = caseSensitive
        ? Qt::CaseSensitive : Qt::CaseInsensitive;
    const QTextCursor current = editor_->textCursor();
    qsizetype foundIndex = -1;

    if (backwards) {
        const qsizetype boundary = current.selectionStart();
        if (boundary > 0)
            foundIndex = text.lastIndexOf(needle, boundary - 1, sensitivity);
        if (foundIndex < 0)
            foundIndex = text.lastIndexOf(needle, -1, sensitivity);
    } else {
        const qsizetype boundary = current.selectionEnd();
        foundIndex = text.indexOf(needle, boundary, sensitivity);
        if (foundIndex < 0)
            foundIndex = text.indexOf(needle, 0, sensitivity);
    }

    const qsizetype maximumPosition = std::numeric_limits<int>::max();
    if (foundIndex < 0 || foundIndex > maximumPosition
        || needle.size() > maximumPosition - foundIndex) {
        statusBar()->showMessage(tr("“%1” was not found.").arg(needle), 3000);
        return false;
    }

    QTextCursor found(editor_->document());
    found.setPosition(static_cast<int>(foundIndex));
    found.setPosition(static_cast<int>(foundIndex + needle.size()),
                      QTextCursor::KeepAnchor);
    editor_->setTextCursor(found);
    editor_->ensureCursorVisible();
    return true;
}

bool TpadMainWindow::replaceCurrentOrNext(const QString &needle,
                                          const QString &replacement,
                                          bool caseSensitive)
{
    if (needle.isEmpty())
        return false;
    const Qt::CaseSensitivity sensitivity = caseSensitive
        ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QTextCursor cursor = editor_->textCursor();
    const bool selectionMatches = cursor.hasSelection()
        && selectedTextWithNewlines(cursor).compare(needle, sensitivity) == 0;
    if (!selectionMatches) {
        if (!findNext(needle, caseSensitive))
            return false;
        cursor = editor_->textCursor();
    }

    cursor.beginEditBlock();
    cursor.insertText(replacement);
    cursor.endEditBlock();
    editor_->setTextCursor(cursor);
    return true;
}

void TpadMainWindow::replaceAll(const QString &needle,
                                const QString &replacement,
                                bool caseSensitive)
{
    if (needle.isEmpty())
        return;
    const QString text = editor_->toPlainText();
    const Qt::CaseSensitivity sensitivity = caseSensitive
        ? Qt::CaseSensitive : Qt::CaseInsensitive;
    std::vector<qsizetype> matches;
    qsizetype offset = 0;
    for (;;) {
        const qsizetype match = text.indexOf(needle, offset, sensitivity);
        if (match < 0)
            break;
        matches.push_back(match);
        offset = match + needle.size();
    }

    QTextCursor editCursor(editor_->document());
    editCursor.beginEditBlock();
    for (auto match = matches.crbegin(); match != matches.crend(); ++match) {
        editCursor.setPosition(static_cast<int>(*match));
        editCursor.setPosition(static_cast<int>(*match + needle.size()),
                               QTextCursor::KeepAnchor);
        editCursor.insertText(replacement);
    }
    editCursor.endEditBlock();
    statusBar()->showMessage(
        tr("Replaced %n occurrence(s).", nullptr,
           static_cast<int>(matches.size())), 3000);
}

void TpadMainWindow::transformSelection(int operation)
{
    QTextCursor cursor = editor_->textCursor();
    const bool selected = cursor.hasSelection();
    if (!selected)
        cursor.select(QTextCursor::Document);
    const int start = cursor.selectionStart();
    QString source = selectedTextWithNewlines(cursor);
    QString result;
    if (operation == 1)
        result = source.toUpper();
    else if (operation == 2)
        result = source.toLower();
    else {
        const auto sourceCodePoints = source.toUcs4();
        std::vector<char32_t> codePoints;
        codePoints.reserve(static_cast<std::size_t>(sourceCodePoints.size()));
        for (uint codePoint : sourceCodePoints)
            codePoints.push_back(static_cast<char32_t>(codePoint));
        std::reverse(codePoints.begin(), codePoints.end());
        result = QString::fromUcs4(codePoints.data(),
                                   static_cast<qsizetype>(codePoints.size()));
    }
    cursor.beginEditBlock();
    cursor.insertText(result);
    cursor.endEditBlock();
    if (selected) {
        cursor.setPosition(start);
        const qsizetype room = std::numeric_limits<int>::max() - start;
        cursor.setPosition(start + static_cast<int>(qMin(result.size(), room)),
                           QTextCursor::KeepAnchor);
    }
    editor_->setTextCursor(cursor);
}

void TpadMainWindow::showTextStatistics()
{
    QString text = selectedTextWithNewlines(editor_->textCursor());
    const bool selection = !text.isEmpty();
    if (!selection)
        text = editor_->toPlainText();
    static const QRegularExpression words(
        QStringLiteral("[\\p{L}\\p{N}_]+(?:['’][\\p{L}\\p{N}_]+)*"),
        QRegularExpression::UseUnicodePropertiesOption);
    int wordCount = 0;
    auto matches = words.globalMatch(text);
    while (matches.hasNext()) {
        matches.next();
        ++wordCount;
    }
    const qsizetype lineCount = text.isEmpty()
                                    ? 0 : text.count(QLatin1Char('\n')) + 1;
    QMessageBox::information(
        this, tr("Text Statistics"),
        tr("%1\n\nWords: %2\nCharacters: %3\nLines: %4")
            .arg(selection ? tr("Current selection") : tr("Entire document"))
            .arg(wordCount).arg(text.size()).arg(lineCount));
}

void TpadMainWindow::showFrequencyAnalysis()
{
    QString text = selectedTextWithNewlines(editor_->textCursor());
    if (text.isEmpty())
        text = editor_->toPlainText();

    QHash<char32_t, qint64> counts;
    qint64 total = 0;
    for (char32_t character : text.toUcs4()) {
        if (character == U'\n' || character == U'\r' || character == U'\t'
            || character == U' ')
            continue;
        ++counts[character];
        ++total;
    }
    if (counts.isEmpty()) {
        QMessageBox::information(this, tr("Frequency Analysis"),
                                 tr("There are no non-whitespace characters to analyze."));
        return;
    }

    QList<QPair<char32_t, qint64>> rows;
    rows.reserve(counts.size());
    for (auto it = counts.cbegin(); it != counts.cend(); ++it)
        rows.append({it.key(), it.value()});
    std::sort(rows.begin(), rows.end(), [](const auto &left, const auto &right) {
        return left.second == right.second ? left.first < right.first
                                            : left.second > right.second;
    });

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Frequency Analysis"));
    auto *layout = new QVBoxLayout(&dialog);
    const int rowCount = static_cast<int>(rows.size());
    auto *table = new QTableWidget(rowCount, 4, &dialog);
    table->setHorizontalHeaderLabels({tr("Character"), tr("Code point"),
                                      tr("Count"), tr("Percent")});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    for (int row = 0; row < rows.size(); ++row) {
        const char32_t scalar = rows.at(row).first;
        table->setItem(row, 0,
                       new QTableWidgetItem(QString::fromUcs4(&scalar, 1)));
        table->setItem(row, 1,
                       new QTableWidgetItem(QStringLiteral("U+%1")
                           .arg(static_cast<uint>(scalar), 4, 16, QLatin1Char('0'))
                           .toUpper()));
        table->setItem(row, 2,
                       new QTableWidgetItem(QString::number(rows.at(row).second)));
        table->setItem(row, 3, new QTableWidgetItem(
            QString::number(100.0 * static_cast<double>(rows.at(row).second)
                                / static_cast<double>(total), 'f', 2)
            + QLatin1Char('%')));
    }
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(table);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);
    dialog.resize(650, 500);
    dialog.exec();
}

void TpadMainWindow::chooseFont()
{
    bool accepted = false;
    QFontDialog::FontDialogOptions options;
#if defined(Q_OS_MACOS)
    // The Cocoa font panel can accept a selection without returning the
    // selected font reliably to a modal QFontDialog.  Use Qt's standard
    // chooser on macOS so accepting the dialog always applies the choice.
    options |= QFontDialog::DontUseNativeDialog;
#endif
    const QFont font = QFontDialog::getFont(&accepted, editor_->font(), this,
                                            tr("Select Editor Font"), options);
    if (accepted) {
        editor_->setFont(font);
        // QPlainTextEdit normally mirrors FontChange into its QTextDocument.
        // Set it explicitly as the rendered-text contract as well.
        editor_->document()->setDefaultFont(font);
        QSettings settings;
        settings.setValue(QStringLiteral("editor/font"), font.toString());
        editor_->setTabWidth(qBound(1,
            settings.value(QStringLiteral("editor/tabWidth"), 4).toInt(), 16));
    }
}

void TpadMainWindow::showPreferences()
{
    QSettings settings;
    const bool oldRecentEnabled = settings.value(
        QStringLiteral("files/recentEnabled"), true).toBool();
    const int oldRecentMaximum = qBound(
        0, settings.value(QStringLiteral("files/recentMax"),
                          TpadRecentFiles::DefaultLimit).toInt(),
        TpadRecentFiles::MaximumLimit);
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Tpad Preferences"));
    auto *layout = new QVBoxLayout(&dialog);
    auto *form = new QFormLayout;
    auto *width = new QSpinBox(&dialog);
    auto *height = new QSpinBox(&dialog);
    auto *tabWidth = new QSpinBox(&dialog);
    auto *recentEnabled = new QCheckBox(
        tr("Remember recently opened files"), &dialog);
    auto *recentMaximum = new QSpinBox(&dialog);
    recentMaximum->setObjectName(QStringLiteral("recentMaximumSpinBox"));
    width->setRange(320, 6000);
    height->setRange(240, 6000);
    tabWidth->setRange(1, 16);
    recentMaximum->setRange(0, TpadRecentFiles::MaximumLimit);
    recentMaximum->setSingleStep(10);
    recentMaximum->setSpecialValueText(tr("No limit"));
    width->setValue(settings.value(QStringLiteral("window/defaultWidth"),
                                   kDefaultWidth).toInt());
    height->setValue(settings.value(QStringLiteral("window/defaultHeight"),
                                    kDefaultHeight).toInt());
    tabWidth->setValue(settings.value(QStringLiteral("editor/tabWidth"), 4).toInt());
    recentEnabled->setChecked(oldRecentEnabled);
    recentMaximum->setValue(oldRecentMaximum);
    recentMaximum->setEnabled(oldRecentEnabled);
    form->addRow(tr("Default window width:"), width);
    form->addRow(tr("Default window height:"), height);
    form->addRow(tr("Tab width (spaces):"), tabWidth);
    form->addRow(recentEnabled);
    form->addRow(tr("Maximum recent files (0 for unlimited):"), recentMaximum);
    connect(recentEnabled, &QCheckBox::toggled,
            recentMaximum, &QWidget::setEnabled);
    layout->addLayout(form);
    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttons, &QDialogButtonBox::accepted, &dialog,
            [&dialog, width, height, tabWidth, recentMaximum] {
                // A click on OK can accept the dialog before a spin box has
                // processed text that is still in its line editor.  Commit
                // every pending, valid edit before reading value() below.
                width->interpretText();
                height->interpretText();
                tabWidth->interpretText();
                recentMaximum->interpretText();
                dialog.accept();
            });
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);
    if (dialog.exec() != QDialog::Accepted)
        return;
    settings.setValue(QStringLiteral("window/defaultWidth"), width->value());
    settings.setValue(QStringLiteral("window/defaultHeight"), height->value());
    settings.setValue(QStringLiteral("editor/tabWidth"), tabWidth->value());
    settings.setValue(QStringLiteral("files/recentEnabled"),
                      recentEnabled->isChecked());
    settings.setValue(QStringLiteral("files/recentMax"), recentMaximum->value());
    editor_->setTabWidth(tabWidth->value());
    const bool shouldTrim = recentEnabled->isChecked()
        && (!oldRecentEnabled
            || (recentMaximum->value() > 0
                && (oldRecentMaximum == 0
                    || recentMaximum->value() < oldRecentMaximum)));
    if (shouldTrim) {
        QString error;
        if (!TpadRecentFiles::trim(recentMaximum->value(), &error)) {
            QMessageBox::warning(this, tr("Recent Files"),
                                 tr("Tpad could not apply the recent-files "
                                    "limit.\n\n%1").arg(error));
        }
    }
    updateRecentFilesMenu();
}

void TpadMainWindow::showAbout()
{
    QMessageBox box(this);
    box.setWindowTitle(tr("About Tpad"));
    box.setIconPixmap(QIcon(QStringLiteral(":/icons/tpad-128.png")).pixmap(96, 96));
    box.setTextFormat(Qt::RichText);
    box.setText(tr("<h3>Tpad %1</h3>"
                   "<p>A small, fast text editor for plain text and source files.</p>"
                   "<p>Qt %2 · %3</p>"
                   "<p>Copyright © 2012–2026 Andrew Smith "
                   "(GNA Services Inc)</p>"
                   "<p><a href=\"https://launchpad.net/tpad-project\">"
                   "launchpad.net/tpad-project</a></p>"
                   "<p>GNU General Public License, version 3 or later.</p>")
                    .arg(QCoreApplication::applicationVersion(),
                         QString::fromLatin1(qVersion()),
#if defined(Q_OS_MACOS)
                         tr("macOS")
#elif defined(Q_OS_WIN)
                         tr("Windows")
#else
                         tr("Linux/Unix")
#endif
                         ));
    box.exec();
}

QString TpadMainWindow::actionText() const
{
    const QTextCursor cursor = editor_->textCursor();
    return cursor.hasSelection() ? selectedTextWithNewlines(cursor)
                                 : editor_->toPlainText();
}

void TpadMainWindow::copyDigestToClipboard(int algorithmValue)
{
    const QCryptographicHash::Algorithm algorithm = hashAlgorithm(algorithmValue);
    const QByteArray digest = QCryptographicHash::hash(actionText().toUtf8(), algorithm)
                                  .toHex();
    QGuiApplication::clipboard()->setText(QString::fromLatin1(digest));
    statusBar()->showMessage(tr("%1 copied to the clipboard.").arg(hashName(algorithm)),
                             3000);
}

void TpadMainWindow::copyFileDigestToClipboard(int algorithmValue)
{
    const QString fileName = QFileDialog::getOpenFileName(
        this, tr("Select a File to Hash"),
        currentFile_.isEmpty() ? QDir::homePath() : QFileInfo(currentFile_).path());
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Unable to Read File"), file.errorString());
        return;
    }
    const QCryptographicHash::Algorithm algorithm = hashAlgorithm(algorithmValue);
    QCryptographicHash hash(algorithm);
    while (!file.atEnd()) {
        const QByteArray chunk = file.read(64 * 1024);
        if (chunk.isEmpty() && file.error() != QFileDevice::NoError) {
            QMessageBox::critical(this, tr("Unable to Read File"), file.errorString());
            return;
        }
        hash.addData(chunk);
    }
    QGuiApplication::clipboard()->setText(QString::fromLatin1(hash.result().toHex()));
    statusBar()->showMessage(tr("%1 file hash copied to the clipboard.")
                                 .arg(hashName(algorithm)), 3000);
}

void TpadMainWindow::base64Selection(bool decode)
{
    const QByteArray input = actionText().toUtf8();
    QByteArray output;
    if (decode) {
        const auto result = QByteArray::fromBase64Encoding(
            input, QByteArray::AbortOnBase64DecodingErrors);
        if (!result) {
            QMessageBox::warning(this, tr("Invalid Base64"),
                                 tr("The selected text is not valid Base64 data."));
            return;
        }
        output = result.decoded;
    } else {
        output = input.toBase64();
    }
    QGuiApplication::clipboard()->setText(QString::fromUtf8(output));
    statusBar()->showMessage(tr("Base64 result copied to the clipboard."), 3000);
}

void TpadMainWindow::base64File(bool decode)
{
    const QString fileName = QFileDialog::getOpenFileName(
        this, decode ? tr("Select a Base64 File") : tr("Select a File to Encode"),
        currentFile_.isEmpty() ? QDir::homePath() : QFileInfo(currentFile_).path());
    if (fileName.isEmpty())
        return;
    QByteArray input;
    QString error;
    if (!readFileBytes(fileName, &input, &error)) {
        QMessageBox::critical(this, tr("Unable to Read File"), error);
        return;
    }
    QByteArray output;
    if (decode) {
        const auto result = QByteArray::fromBase64Encoding(
            input, QByteArray::AbortOnBase64DecodingErrors);
        if (!result) {
            QMessageBox::warning(this, tr("Invalid Base64"),
                                 tr("The selected file does not contain valid Base64 data."));
            return;
        }
        output = result.decoded;
    } else {
        output = input.toBase64();
    }

    auto *mime = new QMimeData;
    mime->setData(QStringLiteral("application/octet-stream"), output);
    mime->setText(decode ? QString::fromUtf8(output) : QString::fromLatin1(output));
    QGuiApplication::clipboard()->setMimeData(mime);
    statusBar()->showMessage(tr("Base64 file result copied to the clipboard."), 3000);
}

void TpadMainWindow::copyCurrentPath()
{
    if (currentFile_.isEmpty())
        return;
    QGuiApplication::clipboard()->setText(QDir::toNativeSeparators(currentFile_));
    statusBar()->showMessage(tr("File path copied to the clipboard."), 3000);
}

void TpadMainWindow::setWrapEnabled(bool enabled)
{
    setActionChecked(wrapAction_, enabled);
    editor_->setLineWrapMode(enabled ? QPlainTextEdit::WidgetWidth
                                     : QPlainTextEdit::NoWrap);
}

void TpadMainWindow::setLineNumbersEnabled(bool enabled)
{
    setActionChecked(lineNumbersAction_, enabled);
    editor_->setLineNumbersVisible(enabled);
}

void TpadMainWindow::setSyntaxEnabled(bool enabled)
{
    setActionChecked(syntaxAction_, enabled);
    highlighter_->setSyntaxEnabled(enabled);
}

void TpadMainWindow::setSpellCheckingEnabled(bool enabled)
{
    if (enabled && (spellChecker_ == nullptr || !spellChecker_->isAvailable())) {
        setActionChecked(spellAction_, false);
        highlighter_->setSpellCheckingEnabled(false);
        return;
    }
    setActionChecked(spellAction_, enabled);
    highlighter_->setSpellCheckingEnabled(enabled);
}

void TpadMainWindow::setFullPathEnabled(bool enabled)
{
    setActionChecked(fullPathAction_, enabled);
    updateWindowTitle();
}

void TpadMainWindow::setOpenGuardEnabled(bool enabled)
{
    setActionChecked(openGuardAction_, enabled);
    if (!enabled) {
        const bool hadLock = documentLock_ != nullptr;
        documentLock_.reset();
        if (hadLock)
            statusBar()->showMessage(tr("Concurrent-editing guard disabled."), 2500);
        return;
    }
    if (currentFile_.isEmpty() || documentLock_ != nullptr)
        return;

    bool proceed = true;
    std::unique_ptr<QLockFile> lock = tryDocumentLock(currentFile_, &proceed);
    if (lock != nullptr) {
        documentLock_ = std::move(lock);
        statusBar()->showMessage(tr("Concurrent-editing guard enabled."), 2500);
        return;
    }

    // A checked action must mean this window actually holds the lock.  The
    // user can still choose "Open Anyway" during a later file transition,
    // but enabling the preference on an already-open locked file is rejected.
    setActionChecked(openGuardAction_, false);
}

void TpadMainWindow::setStayOnTop(bool enabled)
{
    setWindowFlag(Qt::WindowStaysOnTopHint, enabled);
    show();
}

void TpadMainWindow::showEditorContextMenu(const QPoint &position)
{
    std::unique_ptr<QMenu> menu(editor_->createStandardContextMenu());
    QTextCursor wordCursor = editor_->cursorForPosition(position);
    wordCursor.select(QTextCursor::WordUnderCursor);
    const QString word = wordCursor.selectedText();
    if (spellAction_->isChecked() && spellChecker_ != nullptr
        && spellChecker_->isAvailable() && !word.isEmpty()
        && !spellChecker_->isCorrect(QStringView(word))) {
        menu->insertSeparator(menu->actions().value(0));
        const QStringList suggestions = spellChecker_->suggestions(QStringView(word));
        if (suggestions.isEmpty()) {
            QAction *none = new QAction(tr("No Spelling Suggestions"), menu.get());
            none->setEnabled(false);
            menu->insertAction(menu->actions().value(0), none);
        } else {
            for (auto it = suggestions.crbegin(); it != suggestions.crend(); ++it) {
                QAction *suggestion = new QAction(*it, menu.get());
                connect(suggestion, &QAction::triggered, this,
                        [this, wordCursor, replacement = *it]() mutable {
                    QTextCursor cursor = wordCursor;
                    cursor.insertText(replacement);
                    editor_->setTextCursor(cursor);
                });
                menu->insertAction(menu->actions().value(0), suggestion);
            }
        }
    }
#if defined(Q_OS_MACOS)
    editor_->setFocus(Qt::MouseFocusReason);
    if (macWritingToolsAvailable()) {
        menu->addSeparator();
        QAction *writingToolsAction = menu->addAction(tr("Writing Tools…"));
        connect(writingToolsAction, &QAction::triggered, this,
                [this] { showMacWritingTools(editor_); });
    }
#endif
    menu->exec(editor_->mapToGlobal(position));
}

void TpadMainWindow::updateFileWatcher()
{
    if (!watcher_->files().isEmpty())
        watcher_->removePaths(watcher_->files());
    if (!currentFile_.isEmpty() && QFileInfo::exists(currentFile_))
        watcher_->addPath(currentFile_);
}

void TpadMainWindow::queueExternalChangeCheck()
{
    if (!saving_)
        watchTimer_->start();
}

void TpadMainWindow::processExternalChange()
{
    updateFileWatcher();
    if (saving_ || currentFile_.isEmpty() || !diskContentChanged())
        return;
    externalChangePending_ = true;
    statusBar()->showMessage(tr("The file changed outside Tpad."));

    switch (askExternalChangeChoice(false)) {
    case ExternalChoice::Reload:
        (void) reloadFile();
        break;
    case ExternalChoice::SaveAs:
        (void) saveFileAs();
        break;
    case ExternalChoice::Overwrite:
        // "Keep Editing" in the asynchronous dialog.  The save path will
        // ask again before replacing the external version.
    case ExternalChoice::Cancel:
        break;
    }
}

bool TpadMainWindow::diskContentChanged() const
{
    if (currentFile_.isEmpty())
        return false;
    bool ok = false;
    const QByteArray current = fileHash(currentFile_, &ok);
    return !ok || current != lastDiskHash_;
}

TpadMainWindow::ExternalChoice
TpadMainWindow::askExternalChangeChoice(bool duringSave)
{
    QMessageBox box(QMessageBox::Warning, tr("File Changed on Disk"),
                    duringSave
                        ? tr("The file was changed by another application. "
                             "Overwriting it may lose those changes.")
                        : tr("The file was changed by another application while "
                             "it was open in Tpad."),
                    QMessageBox::NoButton, this);
    QPushButton *reload = box.addButton(tr("Reload from Disk"),
                                        QMessageBox::DestructiveRole);
    QPushButton *saveAs = box.addButton(tr("Save As…"), QMessageBox::ActionRole);
    QPushButton *overwrite = box.addButton(duringSave ? tr("Overwrite")
                                                       : tr("Keep Editing"),
                                           QMessageBox::AcceptRole);
    QPushButton *cancel = box.addButton(QMessageBox::Cancel);
    box.setDefaultButton(cancel);
    box.exec();
    if (box.clickedButton() == reload)
        return ExternalChoice::Reload;
    if (box.clickedButton() == saveAs)
        return ExternalChoice::SaveAs;
    if (box.clickedButton() == overwrite)
        return ExternalChoice::Overwrite;
    return ExternalChoice::Cancel;
}

QByteArray TpadMainWindow::fileHash(const QString &fileName, bool *ok)
{
    if (ok != nullptr)
        *ok = false;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return {};
    QCryptographicHash hash(QCryptographicHash::Sha512);
    while (!file.atEnd()) {
        const QByteArray chunk = file.read(64 * 1024);
        if (chunk.isEmpty() && file.error() != QFileDevice::NoError)
            return {};
        hash.addData(chunk);
    }
    if (ok != nullptr)
        *ok = true;
    return hash.result();
}

std::unique_ptr<QLockFile>
TpadMainWindow::tryDocumentLock(const QString &fileName, bool *proceed)
{
    if (proceed != nullptr)
        *proceed = false;
    if (fileName.isEmpty())
        return {};

    auto lock = std::make_unique<QLockFile>(documentLockPath(fileName));
    // A text editor can legitimately remain open for days.  QLockFile still
    // checks local owner PIDs, but must not expire a live document by age.
    lock->setStaleLockTime(0);
    if (lock->tryLock(0)) {
        if (proceed != nullptr)
            *proceed = true;
        return lock;
    }

    qint64 processId = 0;
    QString host;
    QString application;
    QString owner;
    if (lock->getLockInfo(&processId, &host, &application)) {
        owner = tr("\n\nOwner: %1 (PID %2) on %3")
                    .arg(application.isEmpty() ? tr("another application")
                                               : application)
                    .arg(processId)
                    .arg(host.isEmpty() ? tr("this computer") : host);
    }

    QMessageBox box(QMessageBox::Warning, tr("File May Already Be Open"),
                    tr("Tpad could not acquire the editing guard for “%1”. "
                       "Editing the same file in more than one application can "
                       "cause lost changes.%2")
                        .arg(QDir::toNativeSeparators(fileName), owner),
                    QMessageBox::NoButton, this);
    QPushButton *openAnyway = box.addButton(tr("Open Anyway"),
                                            QMessageBox::DestructiveRole);
    QPushButton *cancel = box.addButton(tr("Cancel"), QMessageBox::RejectRole);
    box.setDefaultButton(cancel);
    box.exec();
    if (proceed != nullptr)
        *proceed = box.clickedButton() == openAnyway;
    return {};
}

QString TpadMainWindow::documentLockPath(const QString &fileName)
{
    const QFileInfo info(fileName);
    return info.dir().filePath(QStringLiteral(".%1.tpad.lock")
                                   .arg(info.fileName()));
}

bool TpadMainWindow::decodeDocument(const QByteArray &bytes, QString *text,
                                    bool *hadUtf8Bom, LineEnding *lineEnding,
                                    bool *requiresEncodingConversion,
                                    QString *error)
{
    if (text == nullptr || hadUtf8Bom == nullptr || lineEnding == nullptr
        || requiresEncodingConversion == nullptr)
        return false;
    *hadUtf8Bom = false;
    *requiresEncodingConversion = false;

    QByteArray payload = bytes;
    QString decoded;
    if (payload.startsWith(QByteArray("\xEF\xBB\xBF", 3))) {
        *hadUtf8Bom = true;
        payload.remove(0, 3);
        QStringDecoder decoder(QStringConverter::Utf8);
        decoded = decoder(payload);
        if (decoder.hasError()) {
            if (error != nullptr)
                *error = tr("The UTF-8 BOM is followed by invalid UTF-8 data.");
            return false;
        }
    } else if (payload.startsWith(QByteArray("\xFF\xFE", 2))) {
        payload.remove(0, 2);
        QStringDecoder decoder(QStringConverter::Utf16LE);
        decoded = decoder(payload);
        if (decoder.hasError()) {
            if (error != nullptr)
                *error = tr("The file contains invalid UTF-16 data.");
            return false;
        }
        // The editor saves Unicode text as UTF-8; preserve an explicit BOM.
        *hadUtf8Bom = true;
        *requiresEncodingConversion = true;
    } else if (payload.startsWith(QByteArray("\xFE\xFF", 2))) {
        payload.remove(0, 2);
        QStringDecoder decoder(QStringConverter::Utf16BE);
        decoded = decoder(payload);
        if (decoder.hasError()) {
            if (error != nullptr)
                *error = tr("The file contains invalid UTF-16 data.");
            return false;
        }
        *hadUtf8Bom = true;
        *requiresEncodingConversion = true;
    } else {
        if (payload.contains('\0')) {
            if (error != nullptr)
                *error = tr("Binary files containing NUL bytes are not supported.");
            return false;
        }
        QStringDecoder decoder(QStringConverter::Utf8);
        decoded = decoder(payload);
        if (decoder.hasError()) {
            decoded = QString::fromLocal8Bit(payload);
            if (decoded.contains(QChar::ReplacementCharacter)) {
                if (error != nullptr)
                    *error = tr("The file is neither valid UTF-8 nor decodable "
                                "with the current system text encoding.");
                return false;
            }
            *requiresEncodingConversion = true;
        }
    }

    if (decoded.contains(QChar::Null)) {
        if (error != nullptr)
            *error = tr("Binary files containing NUL characters are not supported.");
        return false;
    }

    const qsizetype crlf = decoded.count(QStringLiteral("\r\n"));
    QString withoutCrLf = decoded;
    withoutCrLf.remove(QStringLiteral("\r\n"));
    const qsizetype cr = withoutCrLf.count(QLatin1Char('\r'));
    const qsizetype lf = withoutCrLf.count(QLatin1Char('\n'));
    if (crlf >= cr && crlf >= lf && crlf > 0)
        *lineEnding = LineEnding::CrLf;
    else if (cr > lf)
        *lineEnding = LineEnding::Cr;
    else
        *lineEnding = LineEnding::Lf;

    decoded.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    decoded.replace(QLatin1Char('\r'), QLatin1Char('\n'));
    *text = decoded;
    return true;
}

QByteArray TpadMainWindow::encodeDocument() const
{
    QString text = editor_->toPlainText();
    if (lineEnding_ == LineEnding::CrLf)
        text.replace(QStringLiteral("\n"), QStringLiteral("\r\n"));
    else if (lineEnding_ == LineEnding::Cr)
        text.replace(QLatin1Char('\n'), QLatin1Char('\r'));
    QByteArray bytes = text.toUtf8();
    if (writeUtf8Bom_)
        bytes.prepend(QByteArray("\xEF\xBB\xBF", 3));
    return bytes;
}

QString TpadMainWindow::normalizedPath(const QString &fileName)
{
    if (fileName.trimmed().isEmpty())
        return {};
    const QFileInfo info(fileName);
    const QString canonical = info.canonicalFilePath();
    return canonical.isEmpty() ? info.absoluteFilePath() : canonical;
}
