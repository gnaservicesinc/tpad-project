#include "../tpad_backend.h"

#include "tpad_application.h"

#include <QCommandLineParser>
#include <QDir>
#include <QIcon>

#if !defined(TPAD_USE_QT)
#  error "The Qt entry point requires TPAD_USE_QT"
#endif

#ifndef TPAD_VERSION
#  define TPAD_VERSION "unknown"
#endif

int main(int argc, char *argv[])
{
    TpadApplication application(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("GNA Services Inc"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("gnaservicesinc.com"));
    QCoreApplication::setApplicationName(QStringLiteral("Tpad"));
    QCoreApplication::setApplicationVersion(QStringLiteral(TPAD_VERSION));
    application.setWindowIcon(QIcon(QStringLiteral(":/icons/tpad-64.png")));

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QCoreApplication::translate("main", "A small, fast text editor."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(
        QCoreApplication::translate("main", "files"),
        QCoreApplication::translate("main", "Files to open, one window per file."),
        QStringLiteral("[files...]"));
    parser.process(application);

    QStringList files;
    for (const QString &argument : parser.positionalArguments())
        files.append(QDir::current().absoluteFilePath(argument));
    application.openFiles(files);
    return application.exec();
}
