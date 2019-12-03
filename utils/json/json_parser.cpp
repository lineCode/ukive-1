#include "utils/json/json_parser.h"

#include "utils/unicode.h"

#define ADV_CUR(adv)  \
    cur += (adv);     \
    if (cur >= json_str.length()) return false;


namespace utl {

    JSONParser::JSONParser() {
    }

    bool JSONParser::parse(const string8& json_str, ValuePtr* v) {
        index_t cur = 0;
        if (cur >= json_str.length()) return false;

        if (json_str.length() >= 3) {
            if (json_str[0] == 0xEF && json_str[1] == 0xBB && json_str[2] == 0xBF) {
                cur = 3;
            }
        }
        if (cur >= json_str.length()) return false;

        char ch = json_str[cur];
        if (ch == '{') {
            // object
            json::ObjectValue* obj_val;
            if (!parseObject(json_str, cur, &cur, &obj_val)) {
                return false;
            }
            v->reset(obj_val);
        } else if (ch == '[') {
            // array
            json::ArrayValue* arr_val;
            if (!parseArray(json_str, cur, &cur, &arr_val)) {
                return false;
            }
            v->reset(arr_val);
        } else {
            return false;
        }

        return true;
    }

    bool JSONParser::parseObject(
        const string8& json_str, index_t cur, index_t* next, json::ObjectValue** v)
    {
        if (cur >= json_str.length()) return false;

        char ch = json_str[cur];
        if (ch != '{') {
            return false;
        }
        ADV_CUR(1);
        if (cur >= json_str.length()) return false;

        eatWhitespace(json_str, cur, &cur);
        if (cur >= json_str.length()) return false;
        ch = json_str[cur];

        auto obj_val = std::make_unique<json::ObjectValue>();
        if (ch != '}') {
            for (;;) {
                std::string str_val;
                if (!parseString(json_str, cur, &cur, &str_val)) {
                    return false;
                }

                eatWhitespace(json_str, cur, &cur);
                if (cur >= json_str.length()) return false;
                ch = json_str[cur];

                if (ch != ':') {
                    return false;
                }
                ADV_CUR(1);

                json::Value* val;
                if (!parseValue(json_str, cur, &cur, &val)) {
                    return false;
                }
                obj_val->put(str_val, val);
                if (cur >= json_str.length()) return false;
                ch = json_str[cur];
                if (ch == ',') {
                    ADV_CUR(1);
                    eatWhitespace(json_str, cur, &cur);
                } else {
                    break;
                }
            }

            if (ch != '}') {
                return false;
            }
        }

        ++cur;
        *v = obj_val.release();
        *next = cur;
        return true;
    }

    bool JSONParser::parseArray(
        const string8& json_str, index_t cur, index_t* next, json::ArrayValue** v)
    {
        if (cur >= json_str.length()) return false;

        char ch = json_str[cur];
        if (ch != '[') {
            return false;
        }
        ADV_CUR(1);
        if (cur >= json_str.length()) return false;

        eatWhitespace(json_str, cur, &cur);
        if (cur >= json_str.length()) return false;
        ch = json_str[cur];

        auto array_val = std::make_unique<json::ArrayValue>();
        if (ch != ']') {
            for (;;) {
                json::Value* val;
                if (!parseValue(json_str, cur, &cur, &val)) {
                    return false;
                }
                array_val->put(val);
                if (cur >= json_str.length()) return false;
                ch = json_str[cur];
                if (ch == ',') {
                    ADV_CUR(1);
                    eatWhitespace(json_str, cur, &cur);
                } else {
                    break;
                }
            }

            if (ch != ']') {
                return false;
            }
        }

        ++cur;
        *v = array_val.release();
        *next = cur;
        return true;
    }

    bool JSONParser::parseValue(
        const string8& json_str, index_t cur, index_t* next, json::Value** v)
    {
        if (cur >= json_str.length()) return false;

        eatWhitespace(json_str, cur, &cur);
        if (cur >= json_str.length()) return false;
        char ch = json_str[cur];

        if (ch == '"') {
            // string
            std::string str_val;
            if (!parseString(json_str, cur, &cur, &str_val)) {
                return false;
            }
            *v = new json::StringValue(str_val);
        } else if (ch == '{') {
            // object
            json::ObjectValue* obj_val;
            if (!parseObject(json_str, cur, &cur, &obj_val)) {
                return false;
            }
            *v = obj_val;
        } else if (ch == '[') {
            // array
            json::ArrayValue* array_obj;
            if (!parseArray(json_str, cur, &cur, &array_obj)) {
                return false;
            }
            *v = array_obj;
        } else if (startWith(json_str, "true", cur)) {
            // true;
            *v = new json::BoolValue(true);
            cur += 4;
        } else if (startWith(json_str, "false", cur)) {
            // false;
            *v = new json::BoolValue(false);
            cur += 5;
        } else if (startWith(json_str, "null", cur)) {
            // null;
            *v = new json::NullValue();
            cur += 4;
        } else {
            // may be number
            json::Value* num_val;
            if (!parseNumber(json_str, cur, &cur, &num_val)) {
                return false;
            }
            *v = num_val;
        }

        eatWhitespace(json_str, cur, &cur);
        *next = cur;
        return true;
    }

    bool JSONParser::parseNumber(
        const string8& json_str, index_t cur, index_t* next, json::Value** v)
    {
        if (cur >= json_str.length()) return false;

        string8 str;
        bool is_frac = false;
        char ch = json_str[cur];
        if (ch == '-') {
            str.push_back('-');
            ADV_CUR(1);
            ch = json_str[cur];
        }

        // integer
        if (ch != '0') {
            if (!isDigit1_9(ch)) return false;
            str.push_back(ch);
            ADV_CUR(1);
            for (;;) {
                ch = json_str[cur];
                if (!isDigit(ch)) break;
                str.push_back(ch);
                ADV_CUR(1);
            }
        } else {
            str.push_back(ch);
            ADV_CUR(1);
            ch = json_str[cur];
        }

        // fraction
        if (ch == '.') {
            is_frac = true;
            str.push_back(ch);
            ADV_CUR(1);
            ch = json_str[cur];
            if (!isDigit(ch)) return false;
            str.push_back(ch);
            ADV_CUR(1);
            for (;;) {
                ch = json_str[cur];
                if (!isDigit(ch)) break;
                str.push_back(ch);
                ADV_CUR(1);
            }
        }

        // exponent
        string8 exp_str;
        if (ch == 'e' || ch == 'E') {
            is_frac = true;
            //str.push_back(ch);
            ADV_CUR(1);
            ch = json_str[cur];

            if (ch == '+' || ch == '-') {
                exp_str.push_back(ch);
                ADV_CUR(1);
                ch = json_str[cur];
            }

            if (!isDigit(ch)) return false;
            exp_str.push_back(ch);
            ADV_CUR(1);
            for (;;) {
                ch = json_str[cur];
                if (!isDigit(ch)) break;
                exp_str.push_back(ch);
                ADV_CUR(1);
            }
        }

        if (is_frac) {
            double result;
            std::istringstream ss(str);
            if (!(ss >> result)) {
                return false;
            }

            if (!exp_str.empty()) {
                int64_t exp;
                std::istringstream exp_ss(exp_str);
                if (!(exp_ss >> exp)) {
                    return false;
                }
                result *= std::pow(10, exp);
            }
            *v = new json::DoubleValue(result);
        } else {
            int64_t result;
            std::istringstream ss(str);
            if (!(ss >> result)) {
                return false;
            }
            *v = new json::IntegerValue(result);
        }

        *next = cur;
        return true;
    }

    bool JSONParser::parseString(
        const string8& json_str, index_t cur, index_t* next, std::string* val)
    {
        if (cur >= json_str.length()) return false;

        string8 str;
        char ch = json_str[cur];
        if (ch != '"') return false;
        ADV_CUR(1);
        ch = json_str[cur];

        string16 u16_tmp;

        for (;;) {
            if (ch == '\\') {
                ADV_CUR(1);
                ch = json_str[cur];
                if (ch != 'u' && !u16_tmp.empty()) {
                    string8 tmp;
                    Unicode::UTF16ToUTF8(u16_tmp, &tmp);
                    str.append(tmp); u16_tmp.clear();
                }

                switch (ch) {
                case '"': str.push_back('\"'); break;
                case '\\': str.push_back('\\'); break;
                case '/': str.push_back('/'); break;
                case 'b': str.push_back('\b'); break;
                case 'f': str.push_back('\f'); break;
                case 'n': str.push_back('\n'); break;
                case 'r': str.push_back('\r'); break;
                case 't': str.push_back('\t'); break;
                case 'u':
                {
                    uint16_t code = 0;

                    // To UTF-16 LE
                    ADV_CUR(1); ch = json_str[cur];
                    if (!isHexDigit(ch)) return false;
                    code |= uint16_t(getHexVal(ch)) << 12;

                    ADV_CUR(1); ch = json_str[cur];
                    if (!isHexDigit(ch)) return false;
                    code |= uint16_t(getHexVal(ch)) << 8;

                    ADV_CUR(1); ch = json_str[cur];
                    if (!isHexDigit(ch)) return false;
                    code |= uint16_t(getHexVal(ch)) << 4;

                    ADV_CUR(1); ch = json_str[cur];
                    if (!isHexDigit(ch)) return false;
                    code |= uint16_t(getHexVal(ch)) << 0;

                    u16_tmp.append({ code });
                    break;
                }
                default: return false;
                }
                ADV_CUR(1);
                ch = json_str[cur];
            } else if (ch == '"') {
                if (!u16_tmp.empty()) {
                    string8 tmp;
                    Unicode::UTF16ToUTF8(u16_tmp, &tmp);
                    str.append(tmp); u16_tmp.clear();
                }

                ADV_CUR(1);
                break;
            } else {
                if (!u16_tmp.empty()) {
                    string8 tmp;
                    Unicode::UTF16ToUTF8(u16_tmp, &tmp);
                    str.append(tmp); u16_tmp.clear();
                }

                str.push_back(ch);
                ADV_CUR(1);
                ch = json_str[cur];
            }
        }

        *val = std::move(str);
        *next = cur;
        return true;
    }

    void JSONParser::eatWhitespace(const string8& json_str, index_t cur, index_t* next) {
        for (; cur < json_str.length(); ++cur) {
            auto ch = json_str[cur];
            if (ch != ' ' && ch != '\r' && ch != '\n' && ch != '\t') {
                break;
            }
        }
        *next = cur;
    }

    bool JSONParser::isDigit(char ch) const {
        return ch >= '0' && ch <= '9';
    }

    bool JSONParser::isDigit1_9(char ch) const {
        return ch >= '1' && ch <= '9';
    }

    bool JSONParser::isHexDigit(char ch) const {
        return isDigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
    }

    uint8_t JSONParser::getHexVal(char ch) const {
        if (ch >= '0' && ch <= '9') {
            return ch - '0';
        }
        if (ch >= 'a' && ch <= 'f') {
            return 10 + (ch - 'a');
        }
        return 10 + (ch - 'A');
    }

}