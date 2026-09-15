#include "spell_checker.h"

#include <QtGlobal>

#if defined(Q_OS_WIN)
#  if defined(__has_include)
#    if __has_include(<spellcheck.h>)
#      define TPAD_HAVE_WINDOWS_SPELLCHECK 1
#    endif
#  endif
#endif

#if defined(TPAD_HAVE_WINDOWS_SPELLCHECK)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <objbase.h>
#  include <spellcheck.h>
#  include <cwchar>
#endif

namespace {

#if defined(TPAD_HAVE_WINDOWS_SPELLCHECK)

class PlatformSpellChecker final : public SpellChecker
{
public:
    PlatformSpellChecker()
    {
        comResult_ = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (FAILED(comResult_) && comResult_ != RPC_E_CHANGED_MODE)
            return;

        ISpellCheckerFactory *factory = nullptr;
        if (FAILED(CoCreateInstance(CLSID_SpellCheckerFactory, nullptr,
                                    CLSCTX_INPROC_SERVER,
                                    IID_ISpellCheckerFactory,
                                    reinterpret_cast<void **>(&factory))))
            return;

        wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {};
        if (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
            copyLocale(localeName, L"en-US");

        BOOL supported = FALSE;
        if (FAILED(factory->IsSupported(localeName, &supported)) || !supported) {
            copyLocale(localeName, L"en-US");
            supported = FALSE;
            (void) factory->IsSupported(localeName, &supported);
        }
        if (supported
            && SUCCEEDED(factory->CreateSpellChecker(localeName, &checker_)))
            language_ = QString::fromWCharArray(localeName);
        factory->Release();
    }

    ~PlatformSpellChecker() override
    {
        if (checker_ != nullptr)
            checker_->Release();
        if (comResult_ == S_OK || comResult_ == S_FALSE)
            CoUninitialize();
    }

    bool isAvailable() const noexcept override { return checker_ != nullptr; }

    bool isCorrect(QStringView word) const override
    {
        if (checker_ == nullptr || word.isEmpty())
            return true;
        const QString owned = word.toString();
        IEnumSpellingError *errors = nullptr;
        if (FAILED(checker_->Check(
                       reinterpret_cast<const wchar_t *>(owned.utf16()), &errors))
            || errors == nullptr)
            return true;
        ISpellingError *error = nullptr;
        const HRESULT next = errors->Next(&error);
        if (error != nullptr)
            error->Release();
        errors->Release();
        return next != S_OK;
    }

    QStringList suggestions(QStringView word) const override
    {
        QStringList result;
        if (checker_ == nullptr || word.isEmpty())
            return result;
        const QString owned = word.toString();
        IEnumString *values = nullptr;
        if (FAILED(checker_->Suggest(
                       reinterpret_cast<const wchar_t *>(owned.utf16()), &values))
            || values == nullptr)
            return result;

        for (int count = 0; count < 8; ++count) {
            LPOLESTR value = nullptr;
            ULONG fetched = 0;
            if (values->Next(1, &value, &fetched) != S_OK || fetched == 0)
                break;
            result.append(QString::fromWCharArray(value));
            CoTaskMemFree(value);
        }
        values->Release();
        return result;
    }

    QString language() const override { return language_; }

private:
    static void copyLocale(wchar_t (&destination)[LOCALE_NAME_MAX_LENGTH],
                           const wchar_t *source)
    {
        std::wcsncpy(destination, source, LOCALE_NAME_MAX_LENGTH - 1);
        destination[LOCALE_NAME_MAX_LENGTH - 1] = L'\0';
    }

    HRESULT comResult_ = E_FAIL;
    ISpellChecker *checker_ = nullptr;
    QString language_;
};

#else

class PlatformSpellChecker final : public SpellChecker
{
public:
    bool isAvailable() const noexcept override { return false; }
    bool isCorrect(QStringView) const override { return true; }
    QStringList suggestions(QStringView) const override { return {}; }
    QString language() const override { return {}; }
};

#endif

} // namespace

// The Objective-C++ source supplies the implementation on macOS.  Keeping the
// fallback in this translation unit lets every non-Apple target use one source
// list, including MinGW installations whose SDK lacks spellcheck.h.
#if !defined(Q_OS_MACOS)
std::unique_ptr<SpellChecker> SpellChecker::create()
{
    return std::make_unique<PlatformSpellChecker>();
}
#endif
