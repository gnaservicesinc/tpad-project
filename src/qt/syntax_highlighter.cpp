#include "syntax_highlighter.h"

#include "spell_checker.h"

#include <QColor>
#include <QFileInfo>
#include <QSet>
#include <QStringList>
#include <QTextDocument>

#include <limits>
#include <utility>

namespace {

QTextCharFormat makeFormat(const QColor &color, bool bold = false,
                           bool italic = false)
{
    QTextCharFormat result;
    result.setForeground(color);
    result.setFontWeight(bold ? QFont::Bold : QFont::Normal);
    result.setFontItalic(italic);
    return result;
}

QString alternation(const QStringList &words)
{
    QStringList escaped;
    escaped.reserve(words.size());
    for (const QString &word : words)
        escaped.append(QRegularExpression::escape(word));
    return QStringLiteral("\\b(?:%1)\\b").arg(escaped.join(QLatin1Char('|')));
}

int textPosition(qsizetype value)
{
    if (value < 0)
        return -1;
    return static_cast<int>(qMin(value,
        static_cast<qsizetype>(std::numeric_limits<int>::max())));
}

} // namespace

TpadSyntaxHighlighter::TpadSyntaxHighlighter(QTextDocument *document,
                                             SpellChecker *spellChecker)
    : QSyntaxHighlighter(document), spellChecker_(spellChecker)
{
    commentFormat_ = makeFormat(QColor(92, 140, 82), false, true);
    spellingFormat_.setUnderlineColor(QColor(210, 45, 45));
    spellingFormat_.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    rebuildRules();
}

void TpadSyntaxHighlighter::setFileName(const QString &fileName)
{
    const QString suffix = QFileInfo(fileName).suffix().toLower();
    const QString base = QFileInfo(fileName).fileName().toLower();
    Profile next = Profile::Plain;

    static const QSet<QString> cLike = {
        QStringLiteral("c"), QStringLiteral("h"), QStringLiteral("cc"),
        QStringLiteral("cpp"), QStringLiteral("cxx"), QStringLiteral("hpp"),
        QStringLiteral("m"), QStringLiteral("mm"), QStringLiteral("java"),
        QStringLiteral("js"), QStringLiteral("ts"), QStringLiteral("cs"),
        QStringLiteral("go"), QStringLiteral("rs"), QStringLiteral("swift")
    };
    static const QSet<QString> python = {
        QStringLiteral("py"), QStringLiteral("pyw"), QStringLiteral("pyi")
    };
    static const QSet<QString> shell = {
        QStringLiteral("sh"), QStringLiteral("bash"), QStringLiteral("zsh"),
        QStringLiteral("fish"), QStringLiteral("conf"), QStringLiteral("ini")
    };
    static const QSet<QString> markup = {
        QStringLiteral("html"), QStringLiteral("htm"), QStringLiteral("xml"),
        QStringLiteral("svg"), QStringLiteral("xhtml")
    };

    if (cLike.contains(suffix))
        next = Profile::CLike;
    else if (python.contains(suffix))
        next = Profile::Python;
    else if (shell.contains(suffix) || base == QStringLiteral("makefile"))
        next = Profile::Shell;
    else if (suffix == QStringLiteral("json"))
        next = Profile::Json;
    else if (markup.contains(suffix))
        next = Profile::Markup;

    if (profile_ != next) {
        profile_ = next;
        rebuildRules();
    }
    rehighlight();
}

void TpadSyntaxHighlighter::setSyntaxEnabled(bool enabled)
{
    if (syntaxEnabled_ == enabled)
        return;
    syntaxEnabled_ = enabled;
    rehighlight();
}

void TpadSyntaxHighlighter::setSpellCheckingEnabled(bool enabled)
{
    const bool available = spellChecker_ != nullptr && spellChecker_->isAvailable();
    const bool next = enabled && available;
    if (spellEnabled_ == next)
        return;
    spellEnabled_ = next;
    spellingCache_.clear();
    rehighlight();
}

void TpadSyntaxHighlighter::rebuildRules()
{
    rules_.clear();
    commentStart_ = {};
    commentEnd_ = {};

    const QTextCharFormat keyword = makeFormat(QColor(45, 85, 180), true);
    const QTextCharFormat string = makeFormat(QColor(176, 72, 40));
    const QTextCharFormat number = makeFormat(QColor(135, 70, 170));
    const QTextCharFormat function = makeFormat(QColor(35, 125, 145));
    const QTextCharFormat preprocessor = makeFormat(QColor(145, 75, 130));

    auto add = [this](const QString &pattern, const QTextCharFormat &textFormat,
                      QRegularExpression::PatternOptions options = {}) {
        rules_.append({QRegularExpression(pattern, options), textFormat});
    };

    add(QStringLiteral("\\b(?:0[xX][0-9A-Fa-f]+|\\d+(?:\\.\\d+)?)\\b"), number);

    switch (profile_) {
    case Profile::CLike:
        add(alternation({QStringLiteral("alignas"), QStringLiteral("alignof"),
                         QStringLiteral("auto"), QStringLiteral("bool"),
                         QStringLiteral("break"), QStringLiteral("case"),
                         QStringLiteral("catch"), QStringLiteral("char"),
                         QStringLiteral("class"), QStringLiteral("const"),
                         QStringLiteral("constexpr"), QStringLiteral("continue"),
                         QStringLiteral("default"), QStringLiteral("delete"),
                         QStringLiteral("do"), QStringLiteral("double"),
                         QStringLiteral("else"), QStringLiteral("enum"),
                         QStringLiteral("explicit"), QStringLiteral("export"),
                         QStringLiteral("extern"), QStringLiteral("false"),
                         QStringLiteral("final"), QStringLiteral("float"),
                         QStringLiteral("for"), QStringLiteral("friend"),
                         QStringLiteral("if"), QStringLiteral("inline"),
                         QStringLiteral("int"), QStringLiteral("long"),
                         QStringLiteral("namespace"), QStringLiteral("new"),
                         QStringLiteral("nullptr"), QStringLiteral("override"),
                         QStringLiteral("private"), QStringLiteral("protected"),
                         QStringLiteral("public"), QStringLiteral("return"),
                         QStringLiteral("short"), QStringLiteral("signed"),
                         QStringLiteral("sizeof"), QStringLiteral("static"),
                         QStringLiteral("struct"), QStringLiteral("switch"),
                         QStringLiteral("template"), QStringLiteral("this"),
                         QStringLiteral("throw"), QStringLiteral("true"),
                         QStringLiteral("try"), QStringLiteral("typedef"),
                         QStringLiteral("typename"), QStringLiteral("union"),
                         QStringLiteral("unsigned"), QStringLiteral("using"),
                         QStringLiteral("virtual"), QStringLiteral("void"),
                         QStringLiteral("volatile"), QStringLiteral("while")}),
            keyword);
        add(QStringLiteral("(?:\\\"(?:\\\\.|[^\\\"\\\\])*\\\"|'(?:\\\\.|[^'\\\\])*')"), string);
        add(QStringLiteral("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\s*\\()"), function);
        add(QStringLiteral("^\\s*#.*$"), preprocessor);
        add(QStringLiteral("//[^\\n]*"), commentFormat_);
        commentStart_ = QRegularExpression(QStringLiteral("/\\*"));
        commentEnd_ = QRegularExpression(QStringLiteral("\\*/"));
        break;
    case Profile::Python:
        add(alternation({QStringLiteral("and"), QStringLiteral("as"),
                         QStringLiteral("assert"), QStringLiteral("async"),
                         QStringLiteral("await"), QStringLiteral("break"),
                         QStringLiteral("class"), QStringLiteral("continue"),
                         QStringLiteral("def"), QStringLiteral("del"),
                         QStringLiteral("elif"), QStringLiteral("else"),
                         QStringLiteral("except"), QStringLiteral("False"),
                         QStringLiteral("finally"), QStringLiteral("for"),
                         QStringLiteral("from"), QStringLiteral("global"),
                         QStringLiteral("if"), QStringLiteral("import"),
                         QStringLiteral("in"), QStringLiteral("is"),
                         QStringLiteral("lambda"), QStringLiteral("None"),
                         QStringLiteral("nonlocal"), QStringLiteral("not"),
                         QStringLiteral("or"), QStringLiteral("pass"),
                         QStringLiteral("raise"), QStringLiteral("return"),
                         QStringLiteral("True"), QStringLiteral("try"),
                         QStringLiteral("while"), QStringLiteral("with"),
                         QStringLiteral("yield")}), keyword);
        add(QStringLiteral("(?:[rubfRUBF]*)(?:\\\"(?:\\\\.|[^\\\"\\\\])*\\\"|'(?:\\\\.|[^'\\\\])*')"), string);
        add(QStringLiteral("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\s*\\()"), function);
        add(QStringLiteral("#[^\\n]*"), commentFormat_);
        break;
    case Profile::Shell:
        add(alternation({QStringLiteral("case"), QStringLiteral("do"),
                         QStringLiteral("done"), QStringLiteral("elif"),
                         QStringLiteral("else"), QStringLiteral("esac"),
                         QStringLiteral("export"), QStringLiteral("fi"),
                         QStringLiteral("for"), QStringLiteral("function"),
                         QStringLiteral("if"), QStringLiteral("in"),
                         QStringLiteral("local"), QStringLiteral("readonly"),
                         QStringLiteral("select"), QStringLiteral("then"),
                         QStringLiteral("until"), QStringLiteral("while")}),
            keyword);
        add(QStringLiteral("(?:\\\"(?:\\\\.|[^\\\"\\\\])*\\\"|'[^']*')"), string);
        add(QStringLiteral("#[^\\n]*"), commentFormat_);
        break;
    case Profile::Json:
        add(QStringLiteral("\\\"(?:\\\\.|[^\\\"\\\\])*\\\"(?=\\s*:)"), keyword);
        add(QStringLiteral("\\\"(?:\\\\.|[^\\\"\\\\])*\\\""), string);
        add(alternation({QStringLiteral("true"), QStringLiteral("false"),
                         QStringLiteral("null")}), keyword);
        break;
    case Profile::Markup:
        add(QStringLiteral("</?[A-Za-z][^>]*>"), keyword);
        add(QStringLiteral("\\b[A-Za-z_:][-A-Za-z0-9_:.]*(?=\\s*=)"), function);
        add(QStringLiteral("(?:\\\"[^\\\"]*\\\"|'[^']*')"), string);
        commentStart_ = QRegularExpression(QStringLiteral("<!--"));
        commentEnd_ = QRegularExpression(QStringLiteral("-->"));
        break;
    case Profile::Plain:
        break;
    }
}

void TpadSyntaxHighlighter::highlightBlock(const QString &text)
{
    setCurrentBlockState(0);
    if (syntaxEnabled_)
        highlightSyntax(text);
    if (spellEnabled_)
        highlightSpelling(text);
}

void TpadSyntaxHighlighter::highlightSyntax(const QString &text)
{
    for (const Rule &rule : std::as_const(rules_)) {
        auto matches = rule.expression.globalMatch(text);
        while (matches.hasNext()) {
            const QRegularExpressionMatch match = matches.next();
            setFormat(textPosition(match.capturedStart()),
                      textPosition(match.capturedLength()), rule.format);
        }
    }

    if (!commentStart_.isValid() || commentStart_.pattern().isEmpty())
        return;

    int start = previousBlockState() == 1
                    ? 0 : textPosition(text.indexOf(commentStart_));
    while (start >= 0) {
        const QRegularExpressionMatch endMatch = commentEnd_.match(text, start);
        const int end = textPosition(endMatch.capturedStart());
        int length;
        if (end < 0) {
            setCurrentBlockState(1);
            length = textPosition(text.size()) - start;
        } else {
            length = end - start + textPosition(endMatch.capturedLength());
        }
        setFormat(start, length, commentFormat_);
        if (end < 0)
            break;
        start = textPosition(text.indexOf(commentStart_, start + length));
    }
}

void TpadSyntaxHighlighter::highlightSpelling(const QString &text)
{
    static const QRegularExpression words(
        QStringLiteral("\\b\\p{L}[\\p{L}\\p{M}'’\u2010-\u2011-]*\\b"),
        QRegularExpression::UseUnicodePropertiesOption);

    auto matches = words.globalMatch(text);
    while (matches.hasNext()) {
        const QRegularExpressionMatch match = matches.next();
        const QString word = match.captured();
        if (word.size() < 2 || wordIsCorrect(word))
            continue;
        const int start = textPosition(match.capturedStart());
        const int length = textPosition(match.capturedLength());
        QTextCharFormat combined = format(start);
        combined.setUnderlineColor(spellingFormat_.underlineColor());
        combined.setUnderlineStyle(spellingFormat_.underlineStyle());
        setFormat(start, length, combined);
    }
}

bool TpadSyntaxHighlighter::wordIsCorrect(const QString &word)
{
    const QString key = word.toCaseFolded();
    const auto found = spellingCache_.constFind(key);
    if (found != spellingCache_.constEnd())
        return found.value();

    if (spellingCache_.size() >= 4096)
        spellingCache_.clear();
    const bool correct = spellChecker_ == nullptr
                         || spellChecker_->isCorrect(QStringView(word));
    spellingCache_.insert(key, correct);
    return correct;
}
