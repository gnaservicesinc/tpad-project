#ifndef TPAD_QT_MAIN_WINDOW_H
#define TPAD_QT_MAIN_WINDOW_H

#ifndef TPAD_USE_QT
#  error "The Qt frontend must be compiled with TPAD_USE_QT"
#endif

#include <QByteArray>
#include <QMainWindow>
#include <QString>

#include <memory>

class QAction;
class QCloseEvent;
class QDialog;
class QDragEnterEvent;
class QDropEvent;
class QFileSystemWatcher;
class QLabel;
class QLockFile;
class QMenu;
class QPoint;
class QTimer;
class CodeEditor;
class SpellChecker;
class TpadSyntaxHighlighter;

class TpadMainWindow final : public QMainWindow
{
public:
    explicit TpadMainWindow(QWidget *parent = nullptr);
    ~TpadMainWindow() override;

    bool loadFile(const QString &fileName, bool createIfMissing = false);
    bool canAdoptFile() const;

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    enum class LineEnding { Lf, CrLf, Cr };
    enum class ExternalChoice { Overwrite, SaveAs, Reload, Cancel };

    void createActions();
    void createMenus();
    void createToolBar();
    void readSettings();
    void writeSettings();
    void updateRecentFilesMenu();
    void recordRecentFile(const QString &fileName);
    void updateWindowTitle();
    void updateCursorStatus();
    void updateActionState();
    bool hasUnsavedChanges() const;

    void newWindow();
    void openFiles();
    bool saveFile();
    bool saveFileAs();
    bool saveToPath(const QString &fileName,
                    bool allowExternalOverwrite = false);
    bool reloadFile();
    bool maybeSave();
    void printDocument();

    void showFindDialog();
    bool findNext(const QString &needle, bool caseSensitive,
                  bool backwards = false);
    bool replaceCurrentOrNext(const QString &needle,
                              const QString &replacement,
                              bool caseSensitive);
    void replaceAll(const QString &needle, const QString &replacement,
                    bool caseSensitive);
    void transformSelection(int operation);
    void showTextStatistics();
    void showFrequencyAnalysis();
    void chooseFont();
    void showPreferences();
    void showAbout();

    QString actionText() const;
    void copyDigestToClipboard(int algorithm);
    void copyFileDigestToClipboard(int algorithm);
    void base64Selection(bool decode);
    void base64File(bool decode);
    void copyCurrentPath();

    void setWrapEnabled(bool enabled);
    void setLineNumbersEnabled(bool enabled);
    void setSyntaxEnabled(bool enabled);
    void setSpellCheckingEnabled(bool enabled);
    void setFullPathEnabled(bool enabled);
    void setOpenGuardEnabled(bool enabled);
    void setStayOnTop(bool enabled);
    void showEditorContextMenu(const QPoint &position);

    void updateFileWatcher();
    void queueExternalChangeCheck();
    void processExternalChange();
    bool diskContentChanged() const;
    ExternalChoice askExternalChangeChoice(bool duringSave);
    static QByteArray fileHash(const QString &fileName, bool *ok = nullptr);
    std::unique_ptr<QLockFile> tryDocumentLock(const QString &fileName,
                                               bool *proceed);
    static QString documentLockPath(const QString &fileName);

    static bool decodeDocument(const QByteArray &bytes, QString *text,
                               bool *hadUtf8Bom, LineEnding *lineEnding,
                               bool *requiresEncodingConversion,
                               QString *error);
    QByteArray encodeDocument() const;
    static QString normalizedPath(const QString &fileName);

    CodeEditor *editor_;
    std::unique_ptr<SpellChecker> spellChecker_;
    TpadSyntaxHighlighter *highlighter_;
    QFileSystemWatcher *watcher_;
    QTimer *watchTimer_;
    QLabel *cursorLabel_;
    std::unique_ptr<QLockFile> documentLock_;

    QString currentFile_;
    QByteArray lastDiskHash_;
    QByteArray cleanDocumentBytes_;
    LineEnding lineEnding_ = LineEnding::Lf;
    bool writeUtf8Bom_ = false;
    bool saving_ = false;
    bool externalChangePending_ = false;
    bool closing_ = false;

    QAction *saveAction_ = nullptr;
    QAction *reloadAction_ = nullptr;
    QAction *copyPathAction_ = nullptr;
    QAction *undoAction_ = nullptr;
    QAction *redoAction_ = nullptr;
    QAction *cutAction_ = nullptr;
    QAction *copyAction_ = nullptr;
    QAction *wrapAction_ = nullptr;
    QAction *lineNumbersAction_ = nullptr;
    QAction *syntaxAction_ = nullptr;
    QAction *spellAction_ = nullptr;
    QAction *bomAction_ = nullptr;
    QAction *fullPathAction_ = nullptr;
    QAction *openGuardAction_ = nullptr;
    QAction *stayOnTopAction_ = nullptr;
    QMenu *recentFilesMenu_ = nullptr;
    QDialog *findDialog_ = nullptr;
};

#endif
