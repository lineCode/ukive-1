#include "ukive/utils/string_utils.h"

#include <algorithm>

#include <Windows.h>

#include "ukive/utils/unicode.h"


namespace ukive {

    string8 UTF16ToUTF8(const string16& str) {
        string8 utf8_str;
        if (Unicode::UTF16ToUTF8(str, &utf8_str)) {
            return utf8_str;
        }

        return {};
    }

    string16 UTF8ToUTF16(const string8& str) {
        string16 utf16_str;
        if (Unicode::UTF8ToUTF16(str, &utf16_str)) {
            return utf16_str;
        }

        return {};
    }

    string16 ANSIToUTF16(const string8& str) {
        int req_length = ::MultiByteToWideChar(
            CP_ACP, MB_PRECOMPOSED, str.data(), str.length(), nullptr, 0);
        if (req_length <= 0) {
            return {};
        }

        std::unique_ptr<WCHAR[]> w_buffer(new WCHAR[req_length]);

        req_length = ::MultiByteToWideChar(
            CP_ACP, MB_PRECOMPOSED,
            str.data(), str.length(), w_buffer.get(), req_length);
        if (req_length <= 0) {
            return {};
        }

        return string16(w_buffer.get(), req_length);
    }

    char toASCIILowerCase(char ch) {
        if (ch >= 0x41 && ch <= 0x5A) {
            ch += 0x20;
        }
        return ch;
    }

    wchar_t toASCIILowerCase(wchar_t ch) {
        if (ch >= 0x41 && ch <= 0x5A) {
            ch += 0x20;
        }
        return ch;
    }

    string8 toASCIILowerCase(const string8& str) {
        string8 result;
        result.reserve(str.size());
        for (auto ch : str) {
            if (ch >= 0x41 && ch <= 0x5A) {
                ch += 0x20;
            }
            result.push_back(ch);
        }
        return result;
    }

    string16 toASCIILowerCase(const string16& str) {
        string16 result;
        result.reserve(str.size());
        for (auto ch : str) {
            if (ch >= 0x41 && ch <= 0x5A) {
                ch += 0x20;
            }
            result.push_back(ch);
        }
        return result;
    }

    char toASCIIUpperCase(char ch) {
        if (ch >= 0x61 && ch <= 0x7A) {
            ch -= 0x20;
        }
        return ch;
    }

    wchar_t toASCIIUpperCase(wchar_t ch) {
        if (ch >= 0x61 && ch <= 0x7A) {
            ch -= 0x20;
        }
        return ch;
    }

    string8 toASCIIUpperCase(const string8& str) {
        string8 result;
        result.reserve(str.size());
        for (auto ch : str) {
            if (ch >= 0x61 && ch <= 0x7A) {
                ch -= 0x20;
            }
            result.push_back(ch);
        }
        return result;
    }

    string16 toASCIIUpperCase(const string16& str) {
        string16 result;
        result.reserve(str.size());
        for (auto ch : str) {
            if (ch >= 0x61 && ch <= 0x7A) {
                ch -= 0x20;
            }
            result.push_back(ch);
        }
        return result;
    }

    string8 trimString(const string8& str, bool all) {
        auto result = str;
        if (all) {
            for (auto it = result.begin(); it != result.end();) {
                if (*it == ' ') {
                    it = result.erase(it);
                } else {
                    ++it;
                }
            }
        } else {
            auto i = result.find_first_not_of(' ');
            if (i == string8::npos) {
                return "";
            }
            if (i > 0) {
                result.erase(0, i);
            }

            i = result.find_last_not_of(' ');
            if (i == string8::npos) {
                return "";
            }
            if (i + 1 < result.length()) {
                result.erase(i + 1);
            }
        }

        return result;
    }

    string16 trimString(const string16& str, bool all) {
        auto result = str;
        if (all) {
            for (auto it = result.begin(); it != result.end();) {
                if (*it == L' ') {
                    it = result.erase(it);
                } else {
                    ++it;
                }
            }
        } else {
            auto i = result.find_first_not_of(L' ');
            if (i == string8::npos) {
                return L"";
            }
            if (i > 0) {
                result.erase(0, i);
            }

            i = result.find_last_not_of(L' ');
            if (i == string8::npos) {
                return L"";
            }
            if (i + 1 < result.length()) {
                result.erase(i + 1);
            }
        }

        return result;
    }

    std::vector<string8> splitString(const string8& str, const string8& token, bool filter_empty) {
        string8::size_type prev_index = 0;
        std::vector<string8> str_vec;
        for (;;) {
            auto cur_index = str.find_first_of(token, prev_index);
            if (cur_index != string8::npos) {
                auto indiv = str.substr(prev_index, cur_index - prev_index);
                if (!filter_empty || !indiv.empty()) {
                    str_vec.push_back(indiv);
                }
                prev_index = cur_index + 1;
            } else {
                auto indiv = str.substr(prev_index, str.length() - prev_index);
                if (!filter_empty || !indiv.empty()) {
                    str_vec.push_back(indiv);
                }
                break;
            }
        }

        return str_vec;
    }

    std::vector<string16> splitString(const string16& str, const string16& token, bool filter_empty) {
        string8::size_type prev_index = 0;
        std::vector<string16> str_vec;
        for (;;) {
            auto cur_index = str.find_first_of(token, prev_index);
            if (cur_index != string16::npos) {
                auto indiv = str.substr(prev_index, cur_index - prev_index);
                if (!filter_empty || !indiv.empty()) {
                    str_vec.push_back(indiv);
                }
                prev_index = cur_index + 1;
            } else {
                auto indiv = str.substr(prev_index, str.length() - prev_index);
                if (!filter_empty || !indiv.empty()) {
                    str_vec.push_back(indiv);
                }
                break;
            }
        }

        return str_vec;
    }

    bool startWith(
        const string8& base,
        const string8& match,
        string8::size_type off,
        bool case_sensitive)
    {
        for (string8::size_type i = 0; i < match.length(); ++i) {
            if (off >= base.length()) return false;
            if (!isEqual(base[off], match[i], case_sensitive)) return false;
            ++off;
        }
        return true;
    }

    bool startWith(
        const string16& base,
        const string16& match,
        string16::size_type off,
        bool case_sensitive)
    {
        for (string16::size_type i = 0; i < match.length(); ++i) {
            if (off >= base.length()) return false;
            if (!isEqual(base[off], match[i], case_sensitive)) return false;
            ++off;
        }
        return true;
    }

    bool endWith(
        const string8& base,
        const string8& match,
        bool case_sensitive)
    {
        int off = base.length();
        for (string8::size_type i = match.length(); i > 0; --i) {
            if (off == 0) return false;
            if (!isEqual(base[off - 1], match[i - 1], case_sensitive)) return false;
            --off;
        }
        return true;
    }

    bool endWith(
        const string16& base,
        const string16& match,
        bool case_sensitive)
    {
        int off = base.length();
        for (string16::size_type i = match.length(); i > 0; --i) {
            if (off == 0) return false;
            if (!isEqual(base[off - 1], match[i - 1], case_sensitive)) return false;
            --off;
        }
        return true;
    }

    string8 replaceAll(const string8& base, const string8& token, const string8& new_s) {
        if (base.empty()) return string8();
        if (token.empty()) return base;

        string8 result = base;
        string8::size_type i = 0;
        for (;;) {
            i = result.find_first_of(token, i);
            if (i != string8::npos) {
                result.replace(i, token.length(), new_s);
                i += new_s.length();
                if (i >= result.length()) {
                    break;
                }
            } else {
                break;
            }
        }
        return result;
    }

    string16 replaceAll(const string16& base, const string16& token, const string16& new_s) {
        if (base.empty()) return string16();
        if (token.empty()) return base;

        string16 result = base;
        string16::size_type i = 0;
        for (;;) {
            i = result.find_first_of(token, i);
            if (i != string16::npos) {
                result.replace(i, token.length(), new_s);
                i += new_s.length();
                if (i >= result.length()) {
                    break;
                }
            } else {
                break;
            }
        }
        return result;
    }

    bool isEqual(char ch1, char ch2, bool case_sensitive) {
        if (case_sensitive) return ch1 == ch2;
        return toASCIILowerCase(ch1) == toASCIILowerCase(ch2);
    }

    bool isEqual(wchar_t ch1, wchar_t ch2, bool case_sensitive) {
        if (case_sensitive) return ch1 == ch2;
        return toASCIILowerCase(ch1) == toASCIILowerCase(ch2);
    }

    bool isEqual(const string8& str1, const string8& str2, bool case_sensitive) {
        if (case_sensitive) return str1 == str2;
        return toASCIILowerCase(str1) == toASCIILowerCase(str2);
    }

    bool isEqual(const string16& str1, const string16& str2, bool case_sensitive) {
        if (case_sensitive) return str1 == str2;
        return toASCIILowerCase(str1) == toASCIILowerCase(str2);
    }

}
