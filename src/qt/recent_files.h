#ifndef TPAD_QT_RECENT_FILES_H
#define TPAD_QT_RECENT_FILES_H

#include <QString>
#include <QStringList>

namespace TpadRecentFiles {

inline constexpr int DefaultLimit = 100;
inline constexpr int MaximumLimit = 1000000;

QString storagePath();
QStringList load(QString *error = nullptr);
bool add(const QString &path, int maximum, QString *error = nullptr);
bool trim(int maximum, QString *error = nullptr);

} // namespace TpadRecentFiles

#endif
