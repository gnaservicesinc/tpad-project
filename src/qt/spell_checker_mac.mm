#include "spell_checker.h"

#if !defined(Q_OS_MACOS)
#  error "spell_checker_mac.mm is only for macOS Qt builds"
#endif

#import <AppKit/NSSpellChecker.h>
#import <Foundation/Foundation.h>

namespace {

NSString *toNativeString(QStringView string)
{
    return [NSString stringWithCharacters:
                     reinterpret_cast<const unichar *>(string.utf16())
                                  length:static_cast<NSUInteger>(string.size())];
}

QString fromNativeString(NSString *string)
{
    if (string == nil)
        return {};
    return QString::fromUtf8([string UTF8String]);
}

class MacSpellChecker final : public SpellChecker
{
public:
    bool isAvailable() const noexcept override
    {
        @autoreleasepool {
            return [NSSpellChecker sharedSpellChecker] != nil;
        }
    }

    bool isCorrect(QStringView word) const override
    {
        if (word.isEmpty())
            return true;
        @autoreleasepool {
            NSSpellChecker *checker = [NSSpellChecker sharedSpellChecker];
            if (checker == nil)
                return true;
            NSString *native = toNativeString(word);
            const NSRange misspelling =
                [checker checkSpellingOfString:native
                                    startingAt:0
                                      language:nil
                                          wrap:NO
                        inSpellDocumentWithTag:0
                                     wordCount:nullptr];
            return misspelling.location == NSNotFound;
        }
    }

    QStringList suggestions(QStringView word) const override
    {
        QStringList result;
        if (word.isEmpty())
            return result;
        @autoreleasepool {
            NSSpellChecker *checker = [NSSpellChecker sharedSpellChecker];
            if (checker == nil)
                return result;
            NSString *native = toNativeString(word);
            NSArray<NSString *> *guesses =
                [checker guessesForWordRange:NSMakeRange(0, native.length)
                                     inString:native
                                     language:nil
                       inSpellDocumentWithTag:0];
            const NSUInteger limit = qMin<NSUInteger>(guesses.count, 8);
            for (NSUInteger index = 0; index < limit; ++index)
                result.append(fromNativeString(guesses[index]));
        }
        return result;
    }

    QString language() const override
    {
        @autoreleasepool {
            NSSpellChecker *checker = [NSSpellChecker sharedSpellChecker];
            return checker != nil ? fromNativeString(checker.language) : QString();
        }
    }
};

} // namespace

std::unique_ptr<SpellChecker> SpellChecker::create()
{
    return std::make_unique<MacSpellChecker>();
}
