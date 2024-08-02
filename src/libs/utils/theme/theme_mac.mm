#include "theme_mac.h"

#include <qglobal.h>
#include <QOperatingSystemVersion>

#include <AppKit/AppKit.h>

#if !QT_MACOS_PLATFORM_SDK_EQUAL_OR_ABOVE(__MAC_10_14)
@interface NSApplication (MojaveForwardDeclarations)
@property (strong) NSAppearance *appearance NS_AVAILABLE_MAC(10_14);
@end
#endif

namespace Utils {
namespace Internal {

bool currentAppearanceMatches(bool dark)
{
#if QT_MACOS_PLATFORM_SDK_EQUAL_OR_ABOVE(__MAC_10_14)
    if (__builtin_available(macOS 10.14, *)) {
        auto appearance = [NSApp.effectiveAppearance
            bestMatchFromAppearancesWithNames:@[NSAppearanceNameAqua, NSAppearanceNameDarkAqua]];
        return
            [appearance isEqualToString:(dark ? NSAppearanceNameDarkAqua : NSAppearanceNameAqua)];
    }
#endif
    return true;
}

void forceMacAppearance(bool dark)
{
    if (currentAppearanceMatches(dark))
        return;
#if __has_builtin(__builtin_available)
    if (__builtin_available(macOS 10.14, *))
#else // Xcode 8
    if (QOperatingSystemVersion::current() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 10, 14, 0))
#endif
        NSApp.appearance = [NSAppearance
            appearanceNamed:(dark ? NSAppearanceNameDarkAqua : NSAppearanceNameAqua)];
}

bool currentAppearanceIsDark()
{
    // double negation, so we get "false" for macOS 10.13
    return !currentAppearanceMatches(false /*==light*/);
}

} // Internal
} // Utils
