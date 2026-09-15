#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileOpenEvent>
#include <QFontMetricsF>
#include <QLineEdit>
#include <QMainWindow>
#include <QMimeData>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QString>
#include <QTemporaryDir>
#include <QTimer>
#include <QUrl>

#include <cmath>
#include <memory>

// This focused source-level test verifies the action/backend invariant.  The
// production class intentionally keeps these implementation details private;
// expose them only in this test translation unit.
#define private public
#include "../tpad_main_window.h"
#undef private

#include "../code_editor.h"
#include "../spell_checker.h"
#include "../syntax_highlighter.h"
#include "../tpad_application.h"

namespace {

int failures = 0;

void expect(bool condition, const char *message)
{
    if (condition)
        return;
    qCritical("FAIL: %s", message);
    ++failures;
}

void writeTestFile(const QString &path)
{
    QFile file(path);
    expect(file.open(QIODevice::WriteOnly | QIODevice::Truncate),
           "test document opens for writing");
    if (file.isOpen()) {
        expect(file.write("one\ntwo\n") == 8,
               "test document is written completely");
    }
}

void verifyCase(const QString &documentPath, bool enabled, int tabWidth)
{
    QSettings settings;
    settings.clear();

    QFont savedFont(QStringLiteral("DejaVu Sans Mono"));
    savedFont.setPointSize(enabled ? 15 : 13);
    settings.setValue(QStringLiteral("window/defaultWidth"),
                      enabled ? 931 : 719);
    settings.setValue(QStringLiteral("window/defaultHeight"),
                      enabled ? 847 : 641);
    settings.setValue(QStringLiteral("editor/wrap"), enabled);
    settings.setValue(QStringLiteral("editor/lineNumbers"), enabled);
    settings.setValue(QStringLiteral("editor/syntax"), enabled);
    settings.setValue(QStringLiteral("editor/spelling"), enabled);
    settings.setValue(QStringLiteral("editor/font"), savedFont.toString());
    settings.setValue(QStringLiteral("editor/tabWidth"), tabWidth);
    settings.setValue(QStringLiteral("window/fullPath"), enabled);
    settings.setValue(QStringLiteral("files/openGuard"), enabled);
    settings.sync();

    TpadMainWindow window;
    expect(window.size() == QSize(enabled ? 931 : 719, enabled ? 847 : 641),
           "saved default window size is applied");

    expect(window.wrapAction_->isChecked() == enabled,
           "wrap action matches saved value");
    expect((window.editor_->lineWrapMode() != QPlainTextEdit::NoWrap) == enabled,
           "wrap backend matches saved value");
    expect(window.lineNumbersAction_->isChecked() == enabled,
           "line-number action matches saved value");
    expect(window.editor_->lineNumbersVisible() == enabled,
           "line-number backend matches saved value");
    expect(window.syntaxAction_->isChecked() == enabled,
           "syntax action matches saved value");
    expect(window.highlighter_->syntaxEnabled() == enabled,
           "syntax backend matches saved value");

    const bool expectedSpelling = enabled && window.spellChecker_ != nullptr
                                  && window.spellChecker_->isAvailable();
    expect(window.spellAction_->isChecked() == expectedSpelling,
           "spelling action matches saved value and backend availability");
    expect(window.highlighter_->spellCheckingEnabled() == expectedSpelling,
           "spelling backend matches action");

    expect(window.fullPathAction_->isChecked() == enabled,
           "full-path action matches saved value");
    expect(window.openGuardAction_->isChecked() == enabled,
           "open-guard action matches saved value");
    expect(window.editor_->font().family() == savedFont.family()
               && window.editor_->font().pointSize() == savedFont.pointSize(),
           "saved editor font is applied");
    const qreal expectedTabDistance =
        QFontMetricsF(window.editor_->font()).horizontalAdvance(QLatin1Char(' '))
        * tabWidth;
    expect(std::abs(window.editor_->tabStopDistance() - expectedTabDistance)
               < 0.01,
           "saved tab width is applied using the restored font");

    expect(window.loadFile(documentPath), "test document loads");
    const QString expectedLabel = enabled
        ? documentPath : QFileInfo(documentPath).fileName();
    expect(window.windowTitle().contains(expectedLabel),
           "saved full-path setting controls the loaded-document title");
    expect((window.documentLock_ != nullptr) == enabled,
           "saved open-guard setting controls the document lock");

    // Exercise the same setters through QAction::toggled and verify that UI
    // and backend cannot diverge after startup either.
    window.wrapAction_->setChecked(!enabled);
    expect((window.editor_->lineWrapMode() != QPlainTextEdit::NoWrap) == !enabled,
           "runtime wrap toggle updates backend");
    window.lineNumbersAction_->setChecked(!enabled);
    expect(window.editor_->lineNumbersVisible() == !enabled,
           "runtime line-number toggle updates backend");
    window.syntaxAction_->setChecked(!enabled);
    expect(window.highlighter_->syntaxEnabled() == !enabled,
           "runtime syntax toggle updates backend");
    window.fullPathAction_->setChecked(!enabled);
    const QString toggledLabel = !enabled
        ? documentPath : QFileInfo(documentPath).fileName();
    expect(window.windowTitle().contains(toggledLabel),
           "runtime full-path toggle updates title");
    window.openGuardAction_->setChecked(!enabled);
    expect((window.documentLock_ != nullptr) == !enabled,
           "runtime open-guard toggle updates document lock");
    expect(QSettings().value(QStringLiteral("files/openGuard")).toBool()
               == !enabled,
           "runtime open-guard toggle is saved immediately");
    TpadMainWindow newWindow;
    expect(newWindow.openGuardAction_->isChecked() == !enabled,
           "new window sees runtime open-guard toggle immediately");

    window.spellAction_->setChecked(!expectedSpelling);
    const bool toggledSpelling = !expectedSpelling
                                 && window.spellChecker_ != nullptr
                                 && window.spellChecker_->isAvailable();
    expect(window.spellAction_->isChecked() == toggledSpelling
               && window.highlighter_->spellCheckingEnabled() == toggledSpelling,
           "runtime spelling toggle keeps action and backend synchronized");
}

void verifyTypedRecentMaximum()
{
    QSettings settings;
    settings.clear();
    settings.setValue(QStringLiteral("files/recentMax"), 40);

    TpadMainWindow window;
    bool dialogHandled = false;
    QTimer::singleShot(0, &window, [&dialogHandled] {
        auto *dialog = qobject_cast<QDialog *>(QApplication::activeModalWidget());
        expect(dialog != nullptr, "preferences dialog becomes active");
        if (dialog == nullptr)
            return;

        auto *maximum = dialog->findChild<QSpinBox *>(
            QStringLiteral("recentMaximumSpinBox"));
        expect(maximum != nullptr, "recent-file maximum field is available");
        auto *buttons = dialog->findChild<QDialogButtonBox *>();
        expect(buttons != nullptr, "preferences buttons are available");
        if (maximum == nullptr || buttons == nullptr) {
            dialog->reject();
            return;
        }

        // Keep the typed text pending in QSpinBox's editor.  Pressing OK must
        // explicitly commit it instead of saving the previous value.
        maximum->setKeyboardTracking(false);
        QLineEdit *editor = maximum->findChild<QLineEdit *>();
        expect(editor != nullptr, "recent-file maximum editor is available");
        QPushButton *ok = buttons->button(QDialogButtonBox::Ok);
        expect(ok != nullptr, "preferences OK button is available");
        if (editor == nullptr || ok == nullptr) {
            dialog->reject();
            return;
        }
        editor->selectAll();
        editor->setText(QStringLiteral("73"));
        expect(maximum->value() == 40,
               "typed recent-file maximum remains pending before OK");
        dialogHandled = true;
        ok->click();
    });

    window.showPreferences();
    expect(dialogHandled, "preferences dialog automation completed");
    expect(settings.value(QStringLiteral("files/recentMax")).toInt() == 73,
           "typed recent-file maximum is saved when OK is pressed");
}

void verifyStaleWindowCannotRevertOpenGuard(bool oldValue)
{
    QSettings settings;
    settings.clear();
    settings.setValue(QStringLiteral("files/openGuard"), oldValue);

    TpadMainWindow changedWindow;
    TpadMainWindow staleWindow;
    expect(changedWindow.openGuardAction_->isChecked() == oldValue
               && staleWindow.openGuardAction_->isChecked() == oldValue,
           "existing windows start with the saved open-guard setting");

    const bool newValue = !oldValue;
    changedWindow.openGuardAction_->setChecked(newValue);
    expect(settings.value(QStringLiteral("files/openGuard")).toBool()
               == newValue,
           "open-guard toggle replaces the saved setting immediately");
    expect(staleWindow.openGuardAction_->isChecked() == oldValue,
           "pre-existing window retains its local stale action state");
    expect(staleWindow.close(), "stale window closes successfully");
    expect(settings.value(QStringLiteral("files/openGuard")).toBool()
               == newValue,
           "closing a stale window does not revert the open-guard setting");

    TpadMainWindow newWindow;
    expect(newWindow.openGuardAction_->isChecked() == newValue,
           "new window retains open-guard setting after stale window closes");
}

void verifyDroppedLocalPaths(const QString &documentsPath)
{
    CodeEditor editor;
    QMimeData data;
    const QString filePath = QDir(documentsPath).filePath(
        QStringLiteral("file with spaces.txt"));
    const QString directoryPath = QDir(documentsPath).filePath(
        QStringLiteral("folder"));
    expect(QDir().mkpath(directoryPath), "drop-test directory is created");
    data.setUrls({QUrl::fromLocalFile(filePath),
                  QUrl::fromLocalFile(directoryPath)});

    editor.insertFromMimeData(&data);
    const QString expected = QDir::toNativeSeparators(filePath)
        + QLatin1Char('\n') + QDir::toNativeSeparators(directoryPath);
    expect(editor.toPlainText() == expected,
           "dropped files and directories insert paths instead of URIs");
}

void verifyFindAndReplace()
{
    TpadMainWindow window;
    window.editor_->setPlainText(QStringLiteral("One one ONE"));

    QTextCursor cursor = window.editor_->textCursor();
    cursor.setPosition(0);
    cursor.setPosition(3, QTextCursor::KeepAnchor);
    window.editor_->setTextCursor(cursor);
    expect(window.replaceCurrentOrNext(QStringLiteral("one"),
                                       QStringLiteral("first"), false),
           "replace accepts the currently selected case-insensitive match");
    expect(window.editor_->toPlainText() == QStringLiteral("first one ONE"),
           "replace does not skip the current match");
    window.editor_->undo();
    expect(window.editor_->toPlainText() == QStringLiteral("One one ONE"),
           "one undo reverses a complete replacement");

    cursor = window.editor_->textCursor();
    cursor.setPosition(0);
    cursor.setPosition(3, QTextCursor::KeepAnchor);
    window.editor_->setTextCursor(cursor);
    expect(window.replaceCurrentOrNext(QStringLiteral("one"),
                                       QStringLiteral("exact"), true),
           "case-sensitive replace finds the next exact match");
    expect(window.editor_->toPlainText() == QStringLiteral("One exact ONE"),
           "case-sensitive replace leaves differently-cased text unchanged");

    window.editor_->setPlainText(QStringLiteral("One one ONE"));
    window.replaceAll(QStringLiteral("one"), QStringLiteral("cat"), true);
    expect(window.editor_->toPlainText() == QStringLiteral("One cat ONE"),
           "case-sensitive Replace All replaces only exact matches");
    window.editor_->setPlainText(QStringLiteral("One one ONE"));
    window.replaceAll(QStringLiteral("one"), QStringLiteral("cat"), false);
    expect(window.editor_->toPlainText() == QStringLiteral("cat cat cat"),
           "case-insensitive Replace All replaces every matching spelling");
}

void verifyFileOpenSaveLifecycle(const QString &documentPath)
{
    TpadApplication *application = TpadApplication::instance();
    QFileOpenEvent openEvent(documentPath);
    expect(QCoreApplication::sendEvent(application, &openEvent),
           "platform file-open event is handled");

    QList<TpadMainWindow *> windows;
    for (QWidget *widget : QApplication::topLevelWidgets()) {
        if (auto *window = dynamic_cast<TpadMainWindow *>(widget))
            windows.append(window);
    }
    expect(windows.size() == 1,
           "platform file-open event creates one document window");
    if (windows.size() != 1)
        return;

    TpadMainWindow *window = windows.constFirst();
    application->openFiles({});
    windows.clear();
    for (QWidget *widget : QApplication::topLevelWidgets()) {
        if (auto *candidate = dynamic_cast<TpadMainWindow *>(widget))
            windows.append(candidate);
    }
    expect(windows.size() == 1,
           "empty startup arguments do not add an untitled window");
    expect(window->currentFile_ == QFileInfo(documentPath).canonicalFilePath(),
           "platform-opened document retains its file path");

    window->editor_->insertPlainText(QStringLiteral("saved text\n"));
    expect(window->hasUnsavedChanges(), "edited document is marked unsaved");
    expect(window->saveFile(), "platform-opened document saves in place");
    expect(!window->hasUnsavedChanges(),
           "saved document is clean before quitting");
    expect(window->currentFile_ == QFileInfo(documentPath).canonicalFilePath(),
           "saving in place retains the original path and filename");

    // Simulate a toolkit formatting pass setting its coarse modified flag.
    // The text bytes are still clean and closing must not show a save dialog.
    window->editor_->document()->setModified(true);
    expect(window->maybeSave(),
           "formatting-only modification does not prompt to save");
    expect(window->close(), "clean platform-opened window closes");
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
}

} // namespace

int main(int argc, char **argv)
{
    TpadApplication application(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("GNA Services Inc"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("gnaservicesinc.com"));
    QCoreApplication::setApplicationName(QStringLiteral("TpadSettingsRestoreTest"));

    QTemporaryDir settingsRoot;
    QTemporaryDir documentsRoot;
    expect(settingsRoot.isValid(), "temporary settings directory is available");
    expect(documentsRoot.isValid(), "temporary document directory is available");
    if (!settingsRoot.isValid() || !documentsRoot.isValid())
        return 1;

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       settingsRoot.path());

    const QString disabledDocument =
        documentsRoot.filePath(QStringLiteral("disabled.txt"));
    const QString enabledDocument =
        documentsRoot.filePath(QStringLiteral("enabled.txt"));
    const QString platformDocument =
        documentsRoot.filePath(QStringLiteral("platform-opened.txt"));
    writeTestFile(disabledDocument);
    writeTestFile(enabledDocument);
    QFile platformFile(platformDocument);
    expect(platformFile.open(QIODevice::WriteOnly),
           "platform-open test document is created");
    platformFile.close();
    verifyCase(disabledDocument, false, 3);
    verifyCase(enabledDocument, true, 7);
    verifyTypedRecentMaximum();
    verifyStaleWindowCannotRevertOpenGuard(false);
    verifyStaleWindowCannotRevertOpenGuard(true);
    verifyDroppedLocalPaths(documentsRoot.path());
    verifyFindAndReplace();
    verifyFileOpenSaveLifecycle(platformDocument);

    if (failures != 0) {
        qCritical("%d settings restoration check(s) failed", failures);
        return 1;
    }
    qInfo("All Qt settings restoration checks passed");
    return 0;
}
