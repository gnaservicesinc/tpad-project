#ifndef TPAD_QT_CODE_EDITOR_H
#define TPAD_QT_CODE_EDITOR_H

#ifndef TPAD_USE_QT
#  error "The Qt frontend must be compiled with TPAD_USE_QT"
#endif

#include <QPlainTextEdit>

class QPaintEvent;
class QMimeData;
class QResizeEvent;
class QWidget;

class CodeEditor final : public QPlainTextEdit
{
public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void setLineNumbersVisible(bool visible);
    bool lineNumbersVisible() const noexcept { return lineNumbersVisible_; }
    void setTabWidth(int spaces);
    void insertFromMimeData(const QMimeData *source) override;

    int lineNumberAreaWidth() const;
    void paintLineNumberArea(QPaintEvent *event);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();

    QWidget *lineNumberArea_;
    bool lineNumbersVisible_ = true;
};

#endif
