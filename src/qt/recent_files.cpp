#include "recent_files.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QSet>
#include <QUrl>

#if defined(Q_OS_UNIX)
#  include <cerrno>
#  include <cstring>
#  include <fcntl.h>
#  include <sys/file.h>
#  include <sys/stat.h>
#  include <unistd.h>
#else
#  include <QLockFile>
#  include <memory>
#endif

#include <utility>

namespace {

constexpr auto kHeader = "# Tpad recent files v1\n";

QString normalizedPath(const QString &path)
{
    if (path.trimmed().isEmpty())
        return {};
    const QFileInfo info(path);
    const QString canonical = info.canonicalFilePath();
    return QDir::cleanPath(canonical.isEmpty() ? info.absoluteFilePath()
                                               : canonical);
}

void setError(QString *error, const QString &message)
{
    if (error != nullptr)
        *error = message;
}

class RecentFileLock final
{
public:
    ~RecentFileLock()
    {
#if defined(Q_OS_UNIX)
        if (descriptor_ >= 0) {
            (void) ::flock(descriptor_, LOCK_UN);
            (void) ::close(descriptor_);
        }
#endif
    }

    bool acquire(const QString &listPath, QString *error)
    {
#if defined(Q_OS_UNIX)
        const QString lockPath = listPath + QStringLiteral(".lock");
        const QByteArray nativePath = QFile::encodeName(lockPath);
        int openFlags = O_CREAT | O_RDWR;
#  if defined(O_NOFOLLOW)
        openFlags |= O_NOFOLLOW;
#  endif
        descriptor_ = ::open(nativePath.constData(), openFlags, 0600);
        if (descriptor_ < 0) {
            setError(error, QStringLiteral("Unable to open %1: %2")
                                .arg(QDir::toNativeSeparators(lockPath),
                                     QString::fromLocal8Bit(std::strerror(errno))));
            return false;
        }
        struct stat lockStatus {};
        const int statResult = ::fstat(descriptor_, &lockStatus);
        if (statResult != 0 || !S_ISREG(lockStatus.st_mode)) {
            const int savedError = statResult != 0 ? errno : EINVAL;
            setError(error, QStringLiteral("Unsafe recent-files lock %1: %2")
                                .arg(QDir::toNativeSeparators(lockPath),
                                     QString::fromLocal8Bit(
                                         std::strerror(savedError))));
            (void) ::close(descriptor_);
            descriptor_ = -1;
            return false;
        }
        (void) ::fcntl(descriptor_, F_SETFD, FD_CLOEXEC);
        int result;
        do {
            result = ::flock(descriptor_, LOCK_EX);
        } while (result != 0 && errno == EINTR);
        if (result != 0) {
            setError(error, QStringLiteral("Unable to lock %1: %2")
                                .arg(QDir::toNativeSeparators(lockPath),
                                     QString::fromLocal8Bit(std::strerror(errno))));
            (void) ::close(descriptor_);
            descriptor_ = -1;
            return false;
        }
        return true;
#else
        lock_ = std::make_unique<QLockFile>(
            listPath + QStringLiteral(".qlock"));
        if (lock_->tryLock(2000))
            return true;
        setError(error, QStringLiteral("Unable to lock %1")
                            .arg(QDir::toNativeSeparators(listPath)));
        lock_.reset();
        return false;
#endif
    }

private:
#if defined(Q_OS_UNIX)
    int descriptor_ = -1;
#else
    std::unique_ptr<QLockFile> lock_;
#endif
};

QStringList loadFromPath(const QString &listPath, QString *error)
{
    QFile file(listPath);
    if (!file.exists())
        return {};
    if (!file.open(QIODevice::ReadOnly)) {
        setError(error, QStringLiteral("Unable to read %1: %2")
                            .arg(QDir::toNativeSeparators(listPath),
                                 file.errorString()));
        return {};
    }

    QStringList files;
    QSet<QString> seen;
    const QList<QByteArray> lines = file.readAll().split('\n');
    if (file.error() != QFileDevice::NoError) {
        setError(error, QStringLiteral("Unable to read %1: %2")
                            .arg(QDir::toNativeSeparators(listPath),
                                 file.errorString()));
        return {};
    }
    for (QByteArray line : lines) {
        line = line.trimmed();
        if (line.isEmpty() || line.startsWith('#'))
            continue;
        QString path;
        if (line.startsWith("file:")) {
            const QUrl url = QUrl::fromEncoded(line);
            if (url.isLocalFile())
                path = url.toLocalFile();
        } else {
            const QString legacyPath = QString::fromUtf8(line);
            if (QFileInfo(legacyPath).isAbsolute())
                path = legacyPath;
        }
        path = normalizedPath(path);
        if (!path.isEmpty() && !seen.contains(path)) {
            seen.insert(path);
            files.append(path);
        }
    }
    return files;
}

void applyLimit(QStringList *files, int maximum)
{
    maximum = qBound(0, maximum, TpadRecentFiles::MaximumLimit);
    if (maximum == 0)
        return;
    while (files->size() > static_cast<qsizetype>(maximum))
        files->removeLast();
}

bool writeToPath(const QString &listPath, const QStringList &files,
                 QString *error)
{
    QByteArray serialized(kHeader);
    for (const QString &path : files) {
        serialized.append(QUrl::fromLocalFile(path).toEncoded(QUrl::FullyEncoded));
        serialized.append('\n');
    }

    QSaveFile file(listPath);
    file.setDirectWriteFallback(false);
    if (!file.open(QIODevice::WriteOnly)) {
        setError(error, QStringLiteral("Unable to write %1: %2")
                            .arg(QDir::toNativeSeparators(listPath),
                                 file.errorString()));
        return false;
    }
#if defined(Q_OS_UNIX)
    const QFileDevice::Permissions privatePermissions =
        QFileDevice::ReadOwner | QFileDevice::WriteOwner;
    if (!file.setPermissions(privatePermissions)) {
        setError(error, QStringLiteral("Unable to make %1 private: %2")
                            .arg(QDir::toNativeSeparators(listPath),
                                 file.errorString()));
        file.cancelWriting();
        return false;
    }
#endif
    const qint64 expected = static_cast<qint64>(serialized.size());
    if (file.write(serialized) != expected || !file.commit()) {
        setError(error, QStringLiteral("Unable to save %1: %2")
                            .arg(QDir::toNativeSeparators(listPath),
                                 file.errorString()));
        return false;
    }
    return true;
}

} // namespace

namespace TpadRecentFiles {

QString storagePath()
{
    const QString overridePath = qEnvironmentVariable("TPAD_RECENT_FILE");
    if (!overridePath.isEmpty() && QFileInfo(overridePath).isAbsolute())
        return QDir::cleanPath(overridePath);
    return QDir::home().filePath(QStringLiteral(".tpad.recent"));
}

QStringList load(QString *error)
{
    return loadFromPath(storagePath(), error);
}

bool add(const QString &path, int maximum, QString *error)
{
    const QString canonical = normalizedPath(path);
    if (canonical.isEmpty()) {
        setError(error, QStringLiteral("The recent-file path is empty."));
        return false;
    }

    const QString listPath = storagePath();
    RecentFileLock lock;
    if (!lock.acquire(listPath, error))
        return false;
    QString loadError;
    QStringList files = loadFromPath(listPath, &loadError);
    if (!loadError.isEmpty()) {
        setError(error, loadError);
        return false;
    }
    files.removeAll(canonical);
    files.prepend(canonical);
    applyLimit(&files, maximum);
    return writeToPath(listPath, files, error);
}

bool trim(int maximum, QString *error)
{
    maximum = qBound(0, maximum, MaximumLimit);
    if (maximum == 0 || !QFileInfo::exists(storagePath()))
        return true;

    const QString listPath = storagePath();
    RecentFileLock lock;
    if (!lock.acquire(listPath, error))
        return false;
    QString loadError;
    QStringList files = loadFromPath(listPath, &loadError);
    if (!loadError.isEmpty()) {
        setError(error, loadError);
        return false;
    }
    applyLimit(&files, maximum);
    return writeToPath(listPath, files, error);
}

} // namespace TpadRecentFiles
