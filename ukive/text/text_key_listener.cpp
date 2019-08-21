#include "text_key_listener.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "ukive/system/clipboard_manager.h"
#include "ukive/text/editable.h"


namespace ukive {

    TextKeyListener::TextKeyListener() {
    }

    TextKeyListener::~TextKeyListener() {
    }


    std::wstring TextKeyListener::numpad(int keyCode) {
        switch (keyCode) {
        case VK_NUMPAD0: return L"0";
        case VK_NUMPAD1: return L"1";
        case VK_NUMPAD2: return L"2";
        case VK_NUMPAD3: return L"3";
        case VK_NUMPAD4: return L"4";
        case VK_NUMPAD5: return L"5";
        case VK_NUMPAD6: return L"6";
        case VK_NUMPAD7: return L"7";
        case VK_NUMPAD8: return L"8";
        case VK_NUMPAD9: return L"9";

        case VK_ADD: return L"+";
        case VK_SUBTRACT: return L"-";
        case VK_MULTIPLY: return L"*";
        case VK_DIVIDE: return L"/";
        case VK_DECIMAL: return L".";
        }

        return L"";
    }

    std::wstring TextKeyListener::number(int keyCode) {
        bool isShiftPressed = (::GetKeyState(VK_SHIFT) < 0);

        switch (keyCode) {
        case 0x30: return isShiftPressed ? L")" : L"0";
        case 0x31: return isShiftPressed ? L"!" : L"1";
        case 0x32: return isShiftPressed ? L"@" : L"2";
        case 0x33: return isShiftPressed ? L"#" : L"3";
        case 0x34: return isShiftPressed ? L"$" : L"4";
        case 0x35: return isShiftPressed ? L"%" : L"5";
        case 0x36: return isShiftPressed ? L"^" : L"6";
        case 0x37: return isShiftPressed ? L"&" : L"7";
        case 0x38: return isShiftPressed ? L"*" : L"8";
        case 0x39: return isShiftPressed ? L"(" : L"9";
        }

        return L"";
    }

    std::wstring TextKeyListener::alphabet(int keyCode, bool upper) {
        if (upper) {
            switch (keyCode) {
            case 0x41: return L"A";
            case 0x42: return L"B";
            case 0x43: return L"C";
            case 0x44: return L"D";
            case 0x45: return L"E";
            case 0x46: return L"F";
            case 0x47: return L"G";
            case 0x48: return L"H";
            case 0x49: return L"I";
            case 0x4A: return L"J";
            case 0x4B: return L"K";
            case 0x4C: return L"L";
            case 0x4D: return L"M";
            case 0x4E: return L"N";
            case 0x4F: return L"O";
            case 0x50: return L"P";
            case 0x51: return L"Q";
            case 0x52: return L"R";
            case 0x53: return L"S";
            case 0x54: return L"T";
            case 0x55: return L"U";
            case 0x56: return L"V";
            case 0x57: return L"W";
            case 0x58: return L"X";
            case 0x59: return L"Y";
            case 0x5A: return L"Z";
            }
        } else {
            switch (keyCode) {
            case 0x41: return L"a";
            case 0x42: return L"b";
            case 0x43: return L"c";
            case 0x44: return L"d";
            case 0x45: return L"e";
            case 0x46: return L"f";
            case 0x47: return L"g";
            case 0x48: return L"h";
            case 0x49: return L"i";
            case 0x4A: return L"j";
            case 0x4B: return L"k";
            case 0x4C: return L"l";
            case 0x4D: return L"m";
            case 0x4E: return L"n";
            case 0x4F: return L"o";
            case 0x50: return L"p";
            case 0x51: return L"q";
            case 0x52: return L"r";
            case 0x53: return L"s";
            case 0x54: return L"t";
            case 0x55: return L"u";
            case 0x56: return L"v";
            case 0x57: return L"w";
            case 0x58: return L"x";
            case 0x59: return L"y";
            case 0x5A: return L"z";
            }
        }

        return L"";
    }

    std::wstring TextKeyListener::symbol(int keyCode) {
        bool isShiftPressed = (::GetKeyState(VK_SHIFT) < 0);

        switch (keyCode) {
        case VK_OEM_PLUS: return isShiftPressed ? L"+" : L"=";
        case VK_OEM_COMMA: return isShiftPressed ? L"<" : L",";
        case VK_OEM_MINUS: return isShiftPressed ? L"_" : L"-";
        case VK_OEM_PERIOD: return isShiftPressed ? L">" : L".";
        case VK_OEM_NEC_EQUAL: return L"=";
        case VK_OEM_1: return isShiftPressed ? L":" : L";";
        case VK_OEM_2: return isShiftPressed ? L"?" : L"/";
        case VK_OEM_3: return isShiftPressed ? L"~" : L"`";
        case VK_OEM_4: return isShiftPressed ? L"{" : L"[";
        case VK_OEM_5: return isShiftPressed ? L"|" : L"\\";
        case VK_OEM_6: return isShiftPressed ? L"}" : L"]";
        case VK_OEM_7: return isShiftPressed ? L"\"" : L"'";
        }

        return L"";
    }


    bool TextKeyListener::metaKey(int keyCode, bool down) {
        switch (keyCode) {
        case VK_SHIFT: return true;
        case VK_CONTROL: return true;
        case VK_CAPITAL: return true;
        }

        return false;
    }

    void TextKeyListener::backspace(Editable* editable) {
        if (editable->hasSelection()) {
            editable->remove();
        } else {
            if (editable->getSelectionStart() > 0) {
                uint32_t len = 1;
                uint32_t index = editable->getSelectionStart() - 1;
                wchar_t prev = editable->at(index);
                if (prev == L'\n' && index > 0) {
                    prev = editable->at(index - 1);
                    if (prev == L'\r') {
                        len = 2;
                    }
                }

                editable->remove(editable->getSelectionStart() - len, len);
                editable->setSelection(editable->getSelectionStart() - len);
            }
        }
    }

    void TextKeyListener::newline(Editable* editable) {
        if (editable->hasSelection()) {
            editable->replace(L"\n");
        } else {
            editable->insert(L"\n");
        }
    }


    bool TextKeyListener::isNumpad(int keyCode) {
        return (keyCode >= 0x60 && keyCode <= 0x6F);
    }

    bool TextKeyListener::isNumber(int keyCode) {
        return (keyCode >= 0x30 && keyCode <= 0x39);
    }

    bool TextKeyListener::isAlphabet(int keyCode) {
        return (keyCode >= 0x41 && keyCode <= 0x5A);
    }

    void TextKeyListener::onKeyDown(
        Editable* editable, bool canEdit, bool canSelect, int keyCode)
    {
        bool hasSelection = editable->hasSelection();
        bool isCtrlKeyPressed = (::GetKeyState(VK_CONTROL) < 0);

        if (isCtrlKeyPressed) {
            switch (keyCode) {
            case 0x41:  //a: select all.
                if (canSelect) {
                    editable->setSelection(0, editable->length());
                }
                break;

            case 0x5A:  //z: undo.
                break;

            case 0x58:  //x: cut.
                if (editable->hasSelection()) {
                    ClipboardManager::saveToClipboard(editable->getSelection());
                    editable->remove();
                }
                break;

            case 0x43:  //c: copy.
                if (editable->hasSelection())
                    ClipboardManager::saveToClipboard(editable->getSelection());
                break;

            case 0x56:  //v: paste.
            {
                std::wstring content = ClipboardManager::getFromClipboard();
                if (!content.empty()) {
                    if (editable->hasSelection()) {
                        editable->replace(content);
                    } else {
                        editable->insert(content);
                    }
                }
                break;
            }
            }

            return;
        }

        if (!canEdit) {
            return;
        }

        //数字
        if (isNumber(keyCode)) {
            if (hasSelection) {
                editable->replace(number(keyCode));
            } else {
                editable->insert(number(keyCode));
            }
        }
        //小键盘
        else if (isNumpad(keyCode)) {
            if (hasSelection) {
                editable->replace(numpad(keyCode));
            } else {
                editable->insert(numpad(keyCode));
            }
        }
        //英文字母
        else if (isAlphabet(keyCode)) {
            bool isCapsLocked = (::GetKeyState(VK_CAPITAL) == TRUE ? true : false);
            bool isShiftPressed = (::GetKeyState(VK_SHIFT) < 0);

            if (hasSelection) {
                editable->replace(
                    alphabet(keyCode, isCapsLocked^isShiftPressed));
            } else {
                editable->insert(
                    alphabet(keyCode, isCapsLocked^isShiftPressed));
            }
        } else {
            switch (keyCode) {
            case VK_SPACE:
                if (hasSelection) {
                    editable->replace(L"\040");
                } else {
                    editable->insert(L"\040");
                }
                break;

            case VK_TAB:
                if (hasSelection) {
                    editable->replace(L"\t");
                } else {
                    editable->insert(L"\t");
                }
                break;

            case VK_BACK:
                backspace(editable);
                break;

            case VK_RETURN:
                newline(editable);
                break;

            default:
                std::wstring text = symbol(keyCode);
                if (!text.empty()) {
                    if (hasSelection) {
                        editable->replace(text);
                    } else {
                        editable->insert(text);
                    }
                }
            }
        }
    }

    void TextKeyListener::onKeyUp(
        Editable* editable, bool canEdit, bool canSelect, int keyCode) {
    }

}