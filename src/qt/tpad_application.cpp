#include "tpad_application.h"

#include "tpad_main_window.h"

#include <QEvent>
#include <QFileInfo>
#include <QFileOpenEvent>
#include <QTimer>

#include <utility>

TpadApplication::TpadApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
    setQuitOnLastWindowClosed(true);
}

TpadApplication *TpadApplication::instance()
{
    return static_cast<TpadApplication *>(QCoreApplication::instance());
}

TpadMainWindow *TpadApplication::openWindow(const QString &fileName,
                                            bool createIfMissing)
{
    auto *window = new TpadMainWindow;
    window->setAttribute(Qt::WA_DeleteOnClose);
    windows_.append(window);
    connect(window, &QObject::destroyed, this, [this] { pruneWindows(); });

    window->show();
    if (!fileName.isEmpty())
        window->loadFile(fileName, createIfMissing);
    return window;
}

void TpadApplication::openFiles(const QStringList &fileNames)
{
    if (fileNames.isEmpty()) {
        // macOS can deliver QFileOpenEvent before main() processes its empty
        // command line. Do not add an untitled window after that document
        // window has already been created.
        pruneWindows();
        if (windows_.isEmpty())
            openWindow();
        return;
    }

    for (const QString &fileName : fileNames) {
        if (!fileName.trimmed().isEmpty())
            // Positional command-line paths retain Tpad's historical
            // behavior of creating a named document when it does not exist.
            openWindow(QFileInfo(fileName).absoluteFilePath(), true);
    }
}

void TpadApplication::openFileFromWindow(const QString &fileName,
                                         TpadMainWindow *requester)
{
    if (requester != nullptr && requester->canAdoptFile()) {
        requester->loadFile(fileName);
        return;
    }
    openWindow(fileName);
}

void TpadApplication::createNewWindow()
{
    openWindow();
}

void TpadApplication::requestQuit()
{
    pruneWindows();
    // QWidget::close() returns false when a document rejects its close event.
    // Stop immediately so QCoreApplication::quit() can never bypass an
    // unsaved-change prompt in another window.
    const QList<QPointer<TpadMainWindow>> windows = windows_;
    for (const QPointer<TpadMainWindow> &window : windows) {
        if (window != nullptr && !window->close())
            return;
    }
    quit();
}

bool TpadApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        const auto *openEvent = static_cast<QFileOpenEvent *>(event);
        const QString fileName = !openEvent->file().isEmpty()
                                     ? openEvent->file()
                                     : openEvent->url().toLocalFile();
        if (!fileName.isEmpty()) {
            pruneWindows();
            for (const QPointer<TpadMainWindow> &window : std::as_const(windows_)) {
                if (window != nullptr && window->canAdoptFile()) {
                    window->loadFile(fileName);
                    return true;
                }
            }
            openWindow(fileName);
            return true;
        }
    }
    return QApplication::event(event);
}

void TpadApplication::pruneWindows()
{
    windows_.removeIf([](const QPointer<TpadMainWindow> &window) {
        return window.isNull();
    });
}
