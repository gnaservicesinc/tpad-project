#include "code_editor.h"

#include <QAbstractTextDocumentLayout>
#include <QDir>
#include <QFontDatabase>
#include <QFontMetricsF>
#include <QMimeData>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QResizeEvent>
#include <QTextBlock>
#include <QUrl>

namespace {

class LineNumberArea final : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor)
        : QWidget(editor), editor_(editor)
    {
        setAccessibleName(CodeEditor::tr("Line numbers"));
    }

    QSize sizeHint() const override
    {
        return {editor_->lineNumberAreaWidth(), 0};
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        editor_->paintLineNumberArea(event);
    }

private:
    CodeEditor *editor_;
};

} // namespace

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent), lineNumberArea_(new LineNumberArea(this))
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setCenterOnScroll(true);
    setLineWrapMode(QPlainTextEdit::WidgetWidth);
    setTabWidth(4);

    connect(this, &QPlainTextEdit::blockCountChanged, this,
            [this] { updateLineNumberAreaWidth(); });
    connect(this, &QPlainTextEdit::updateRequest, this,
            [this](const QRect &rect, int dy) { updateLineNumberArea(rect, dy); });
    connect(this, &QPlainTextEdit::cursorPositionChanged, this,
            [this] { highlightCurrentLine(); });

    updateLineNumberAreaWidth();
    highlightCurrentLine();
}

void CodeEditor::setLineNumbersVisible(bool visible)
{
    if (lineNumbersVisible_ == visible)
        return;
    lineNumbersVisible_ = visible;
    lineNumberArea_->setVisible(visible);
    updateLineNumberAreaWidth();
}

void CodeEditor::setTabWidth(int spaces)
{
    const int safeSpaces = qBound(1, spaces, 16);
    setTabStopDistance(QFontMetricsF(font()).horizontalAdvance(QLatin1Char(' '))
                       * safeSpaces);
}

void CodeEditor::insertFromMimeData(const QMimeData *source)
{
    if (source == nullptr)
        return;
    if (!source->hasUrls()) {
        QPlainTextEdit::insertFromMimeData(source);
        return;
    }

    QStringList paths;
    for (const QUrl &url : source->urls()) {
        if (!url.isLocalFile()) {
            QPlainTextEdit::insertFromMimeData(source);
            return;
        }
        paths.append(QDir::toNativeSeparators(url.toLocalFile()));
    }
    if (paths.isEmpty()) {
        QPlainTextEdit::insertFromMimeData(source);
        return;
    }
    textCursor().insertText(paths.join(QLatin1Char('\n')));
}

int CodeEditor::lineNumberAreaWidth() const
{
    if (!lineNumbersVisible_)
        return 0;

    int digits = 1;
    for (int lines = qMax(1, blockCount()); lines >= 10; lines /= 10)
        ++digits;
    return 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void CodeEditor::updateLineNumberAreaWidth()
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy != 0)
        lineNumberArea_->scroll(0, dy);
    else
        lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    const QRect contents = contentsRect();
    lineNumberArea_->setGeometry(contents.left(), contents.top(),
                                 lineNumberAreaWidth(), contents.height());
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> selections;
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor color = palette().color(QPalette::AlternateBase);
        color.setAlpha(120);
        selection.format.setBackground(color);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        selections.append(selection);
    }
    setExtraSelections(selections);
}

void CodeEditor::paintLineNumberArea(QPaintEvent *event)
{
    QPainter painter(lineNumberArea_);
    painter.fillRect(event->rect(), palette().color(QPalette::AlternateBase));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            painter.setPen(palette().color(QPalette::Disabled, QPalette::Text));
            painter.drawText(0, top, lineNumberArea_->width() - 5,
                             fontMetrics().height(), Qt::AlignRight,
                             QString::number(blockNumber + 1));
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
