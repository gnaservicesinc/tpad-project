#ifndef TPAD_QT_SPELL_CHECKER_H
#define TPAD_QT_SPELL_CHECKER_H

#ifndef TPAD_USE_QT
#  error "The Qt frontend must be compiled with TPAD_USE_QT"
#endif

#include <QString>
#include <QStringList>
#include <QStringView>

#include <memory>

class SpellChecker
{
public:
    virtual ~SpellChecker() = default;

    virtual bool isAvailable() const noexcept = 0;
    virtual bool isCorrect(QStringView word) const = 0;
    virtual QStringList suggestions(QStringView word) const = 0;
    virtual QString language() const = 0;

    // Exactly one platform implementation must be part of a Qt target.
    static std::unique_ptr<SpellChecker> create();
};

#endif
