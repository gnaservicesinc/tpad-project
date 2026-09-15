#ifndef TPAD_QT_APPLICATION_H
#define TPAD_QT_APPLICATION_H

#ifndef TPAD_USE_QT
#  error "The Qt frontend must be compiled with TPAD_USE_QT"
#endif

#include <QApplication>
#include <QList>
#include <QPointer>
#include <QStringList>

class TpadMainWindow;

class TpadApplication final : public QApplication
{
public:
    TpadApplication(int &argc, char **argv);

    static TpadApplication *instance();

    TpadMainWindow *openWindow(const QString &fileName = {},
                               bool createIfMissing = false);
    void openFiles(const QStringList &fileNames);
    void openFileFromWindow(const QString &fileName, TpadMainWindow *requester);
    void createNewWindow();
    void requestQuit();

protected:
    bool event(QEvent *event) override;

private:
    void pruneWindows();

    QList<QPointer<TpadMainWindow>> windows_;
};

#endif
