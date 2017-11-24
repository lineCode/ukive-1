#ifndef UKIVE_TEXT_TEXT_KEY_LISTENER_H_
#define UKIVE_TEXT_TEXT_KEY_LISTENER_H_

#include "ukive/utils/string_utils.h"


namespace ukive {

    class Editable;

    class TextKeyListener
    {
    private:
        string16 numpad(int keyCode);
        string16 number(int keyCode);
        string16 alphabet(int keyCode, bool upper);
        string16 symbol(int keyCode);

        bool metaKey(int keyCode, bool down);

        void backspace(Editable *editable);
        void newline(Editable *editable);

        bool isNumpad(int keyCode);
        bool isNumber(int keyCode);
        bool isAlphabet(int keyCode);

    public:
        TextKeyListener();
        ~TextKeyListener();

        void onKeyDown(Editable *editable, bool canEdit, bool canSelect, int keyCode);
        void onKeyUp(Editable *editable, bool canEdit, bool canSelect, int keyCode);
    };

}

#endif  // UKIVE_TEXT_TEXT_KEY_LISTENER_H_