#ifndef UKIVE_UTILS_STRING_UTILS_H_
#define UKIVE_UTILS_STRING_UTILS_H_

#include <string>
#include <vector>
#include <sstream>


namespace ukive {

    typedef std::string string8;
    typedef std::wstring string16;

    string8 UTF16ToUTF8(const string16& str);
    string16 UTF8ToUTF16(const string8& str);

    string16 ANSIToUTF16(const string8& str);

    char toASCIILowerCase(char ch);
    wchar_t toASCIILowerCase(wchar_t ch);
    string8 toASCIILowerCase(const string8& str);
    string16 toASCIILowerCase(const string16& str);

    char toASCIIUpperCase(char ch);
    wchar_t toASCIIUpperCase(wchar_t ch);
    string8 toASCIIUpperCase(const string8& str);
    string16 toASCIIUpperCase(const string16& str);

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

}

using string8 = ukive::string8;
using string16 = ukive::string16;


#endif  // UKIVE_UTILS_STRING_UTILS_H_