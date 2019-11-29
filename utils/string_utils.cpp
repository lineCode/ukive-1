#include "utils/string_utils.h"

#include <algorithm>
#include <cstdarg>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "utils/unicode.h"
#include "utils/stl_utils.h"


namespace utl {

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
            CP_ACP, MB_PRECOMPOSED, str.data(), STLCInt(str.length()), nullptr, 0);
        if (req_length <= 0) {
            return {};
        }

        std::unique_ptr<WCHAR[]> w_buffer(new WCHAR[req_length]);

        req_length = ::MultiByteToWideChar(
            CP_ACP, MB_PRECOMPOSED,
            str.data(), STLCInt(str.length()), w_buffer.get(), req_length);
        if (req_length <= 0) {
            return {};
        }

        return string16(w_buffer.get(), req_length);
    }

    char toLowerCase(char ch) {
        return ::tolower(static_cast<unsigned char>(ch));
    }

    wchar_t toLowerCase(wchar_t ch) {
        return ::towlower(static_cast<wint_t>(ch));
    }

    string8 toLowerCase(const string8& str) {
        string8 result;
        result.resize(str.size());
        std::transform(str.cbegin(), str.cend(), result.begin(), ::tolower);
        return result;
    }

    string16 toLowerCase(const string16& str) {
        string16 result;
        result.resize(str.size());
        std::transform(str.cbegin(), str.cend(), result.begin(), ::towlower);
        return result;
    }

    char toUpperCase(char ch) {
        return ::toupper(static_cast<unsigned char>(ch));
    }

    wchar_t toUpperCase(wchar_t ch) {
        return ::towupper(static_cast<wint_t>(ch));
    }

    string8 toUpperCase(const string8& str) {
        string8 result;
        result.resize(str.size());
        std::transform(str.cbegin(), str.cend(), result.begin(), ::toupper);
        return result;
    }

    string16 toUpperCase(const string16& str) {
        string16 result;
        result.resize(str.size());
        std::transform(str.cbegin(), str.cend(), result.begin(), ::towupper);
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
        auto off = base.length();
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
        auto off = base.length();
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
        return toLowerCase(ch1) == toLowerCase(ch2);
    }

    bool isEqual(wchar_t ch1, wchar_t ch2, bool case_sensitive) {
        if (case_sensitive) return ch1 == ch2;
        return toLowerCase(ch1) == toLowerCase(ch2);
    }

    bool isEqual(const string8& str1, const string8& str2, bool case_sensitive) {
        if (case_sensitive) return str1 == str2;
        return toLowerCase(str1) == toLowerCase(str2);
    }

    bool isEqual(const string16& str1, const string16& str2, bool case_sensitive) {
        if (case_sensitive) return str1 == str2;
        return toLowerCase(str1) == toLowerCase(str2);
    }

    string8 stringPrintf(const char* format, ...) {
        va_list vars;
        va_start(vars, format);
        va_list vars2;
        va_copy(vars2, vars);

        int ret = vsnprintf(nullptr, 0, format, vars);
        va_end(vars);
        if (ret <= 0) {
            return {};
        }

        ++ret;
        std::unique_ptr<char[]> buf(new char[ret]);
        ret = vsnprintf(buf.get(), ret, format, vars2);
        va_end(vars2);
        if (ret <= 0) {
            return {};
        }

        return string8(buf.get(), ret - 1);
    }

    string16 stringPrintf(const wchar_t* format, ...) {
        va_list vars;
        va_start(vars, format);
        va_list vars2;
        va_copy(vars2, vars);

        int ret = vswprintf(nullptr, 0, format, vars);
        va_end(vars);
        if (ret <= 0) {
            return {};
        }

        ++ret;
        std::unique_ptr<wchar_t[]> buf(new wchar_t[ret]);
        ret = vswprintf(buf.get(), ret, format, vars2);
        va_end(vars2);
        if (ret <= 0) {
            return {};
        }

        return string16(buf.get(), ret);
    }

}
