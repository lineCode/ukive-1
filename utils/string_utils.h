#ifndef UTILS_STRING_UTILS_H_
#define UTILS_STRING_UTILS_H_

#include <string>
#include <vector>
#include <sstream>


namespace utl {

    using string8 =  std::string;
    using string16 = std::wstring;
    using stringu8 = std::basic_string<uint8_t, std::char_traits<uint8_t>, std::allocator<uint8_t>>;

    string8 UTF16ToUTF8(const string16& str);
    string16 UTF8ToUTF16(const string8& str);

    string16 ANSIToUTF16(const string8& str);

    char toLowerCase(char ch);
    wchar_t toLowerCase(wchar_t ch);
    string8 toLowerCase(const string8& str);
    string16 toLowerCase(const string16& str);

    char toUpperCase(char ch);
    wchar_t toUpperCase(wchar_t ch);
    string8 toUpperCase(const string8& str);
    string16 toUpperCase(const string16& str);

    string8 trimString(const string8& str, bool all);
    string16 trimString(const string16& str, bool all);

    // 按照 token 中的每个字符分割字符串 str
    std::vector<string8> splitString(
        const string8& str, const string8& token, bool filter_empty = false);
    // 按照 token 中的每个字符分割字符串 str
    std::vector<string16> splitString(
        const string16& str, const string16& token, bool filter_empty = false);

    bool startWith(
        const string8& base,
        const string8& match,
        string8::size_type off = 0,
        bool case_sensitive = true);
    bool startWith(
        const string16& base,
        const string16& match,
        string16::size_type off = 0,
        bool case_sensitive = true);

    bool endWith(
        const string8& base,
        const string8& match,
        bool case_sensitive = true);
    bool endWith(
        const string16& base,
        const string16& match,
        bool case_sensitive = true);

    // 将 token 中的每个在 base 中出现的字符替换为 new_s
    string8 replaceAll(const string8& base, const string8& token, const string8& new_s);
    // 将 token 中的每个在 base 中出现的字符替换为 new_s
    string16 replaceAll(const string16& base, const string16& token, const string16& new_s);

    bool isEqual(char ch1, char ch2, bool case_sensitive = true);
    bool isEqual(wchar_t ch1, wchar_t ch2, bool case_sensitive = true);
    bool isEqual(const string8& str1, const string8& str2, bool case_sensitive = true);
    bool isEqual(const string16& str1, const string16& str2, bool case_sensitive = true);

    string8 stringPrintf(const char* format, ...);
    string16 stringPrintf(const wchar_t* format, ...);

    template <typename T>
    string8 toString8Hex(T i) {
        std::ostringstream ss;
        ss << std::hex << std::uppercase << i;
        return ss.str();
    }

    template <typename T>
    string16 toString16Hex(T i) {
        std::wostringstream ss;
        ss << std::hex << std::uppercase << i;
        return ss.str();
    }

    template <typename T>
    bool stringToNumber(const string8& text, T* out) {
        T result;
        std::istringstream ss(text);
        if (!(ss >> result)) {
            return false;
        }

        *out = result;
        return true;
    }

    template <typename T>
    bool stringToNumber(const string16& text, T* out) {
        T result;
        std::wistringstream ss(text);
        if (!(ss >> result)) {
            return false;
        }

        *out = result;
        return true;
    }

    template <typename T>
    bool hexStringToNumber(const string8& text, T* out) {
        T result;
        std::istringstream ss(text);
        if (!(ss >> std::hex >> result)) {
            return false;
        }

        *out = result;
        return true;
    }

    template <typename T>
    bool hexStringToNumber(const string16& text, T* out) {
        T result;
        std::wistringstream ss(text);
        if (!(ss >> std::hex >> result)) {
            return false;
        }

        *out = result;
        return true;
    }

}

using string8 = utl::string8;
using string16 = utl::string16;
using stringu8 = utl::stringu8;


#endif  // UTILS_STRING_UTILS_H_