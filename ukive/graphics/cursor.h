#ifndef UKIVE_GRAPHICS_CURSOR_H_
#define UKIVE_GRAPHICS_CURSOR_H_


namespace ukive {

    enum Cursor {
        ARROW,
        IBEAM,
        WAIT,
        CROSS,
        UPARROW,
        SIZENWSE,
        SIZENESW,
        SIZEWE,
        SIZENS,
        SIZEALL,
        NO,
        // (WINVER >= 0x0500)
        HAND,
        APPSTARTING,
        // (WINVER >= 0x0400)
        HELP,
        // (WINVER >= 0x0606)
        PIN,
        PERSON
    };

}

#endif  // UKIVE_GRAPHICS_CURSOR_H_