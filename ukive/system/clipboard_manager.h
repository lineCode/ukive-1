#ifndef UKIVE_SYSTEM_CLIPBOARD_MANAGER_H_
#define UKIVE_SYSTEM_CLIPBOARD_MANAGER_H_

#include "utils/string_utils.h"


namespace ukive {

    class ClipboardManager {
    public:
        ClipboardManager();
        ~ClipboardManager();

        static void saveToClipboard(string16 text);
        static string16 getFromClipboard();
    };

}

#endif // !UKIVE_SYSTEM_CLIPBOARD_MANAGER_H_