#ifndef TPAD_QT_SYNTAX_HIGHLIGHTER_H
#define TPAD_QT_SYNTAX_HIGHLIGHTER_H

#ifndef TPAD_USE_QT
#  error "The Qt frontend must be compiled with TPAD_USE_QT"
#endif

#include <QHash>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

class SpellChecker;

class TpadSyntaxHighlighter final : public QSyntaxHighlighter
{
public:
    explicit TpadSyntaxHighlighter(QTextDocument *document,
                                   SpellChecker *spellChecker);

    void setFileName(const QString &fileName);
    void setSyntaxEnabled(bool enabled);
    void setSpellCheckingEnabled(bool enabled);
    bool syntaxEnabled() const noexcept { return syntaxEnabled_; }
    bool spellCheckingEnabled() const noexcept { return spellEnabled_; }

protected:
    void highlightBlock(const QString &text) override;

private:
    struct Rule {
        QRegularExpression expression;
        QTextCharFormat format;
    };

    enum class Profile { Plain, CLike, Python, Shell, Json, Markup };

    void rebuildRules();
    void highlightSyntax(const QString &text);
    void highlightSpelling(const QString &text);
    bool wordIsCorrect(const QString &word);

    SpellChecker *spellChecker_;
    QVector<Rule> rules_;
    QHash<QString, bool> spellingCache_;
    QTextCharFormat commentFormat_;
    QTextCharFormat spellingFormat_;
    QRegularExpression commentStart_;
    QRegularExpression commentEnd_;
    Profile profile_ = Profile::Plain;
    bool syntaxEnabled_ = true;
    bool spellEnabled_ = false;
};

#endif
