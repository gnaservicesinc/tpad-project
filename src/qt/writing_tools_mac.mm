#include "writing_tools_mac.h"

#include <QtCore/qglobal.h>

#if !defined(Q_OS_MACOS)
#  error "writing_tools_mac.mm is only for macOS Qt builds"
#endif

#include <QWidget>

#import <AppKit/AppKit.h>
#import <objc/message.h>

namespace {

SEL writingToolsSelector()
{
    return NSSelectorFromString(@"showWritingTools:");
}

} // namespace

bool macWritingToolsAvailable()
{
    @autoreleasepool {
        Class coordinator = NSClassFromString(@"NSWritingToolsCoordinator");
        const SEL availabilitySelector =
            NSSelectorFromString(@"isWritingToolsAvailable");
        if (coordinator == Nil
            || ![coordinator respondsToSelector:availabilitySelector])
            return false;

        using AvailabilityFunction = BOOL (*)(id, SEL);
        const auto isAvailable =
            reinterpret_cast<AvailabilityFunction>(objc_msgSend);
        if (!isAvailable(coordinator, availabilitySelector))
            return false;

        return [NSApp targetForAction:writingToolsSelector()] != nil;
    }
}

void showMacWritingTools(QWidget *focusWidget)
{
    if (focusWidget != nullptr)
        focusWidget->setFocus(Qt::OtherFocusReason);

    @autoreleasepool {
        // The Writing Tools responder accepts a nullable sender and treats
        // nil as the generic action.  Do not use NSApp here: AppKit queries
        // the sender's menu-item tag, which NSApplication does not provide.
        (void) [NSApp sendAction:writingToolsSelector() to:nil from:nil];
    }
}
