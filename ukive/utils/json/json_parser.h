#ifndef UKIVE_UTILS_JSON_JSON_PARSER_H_
#define UKIVE_UTILS_JSON_JSON_PARSER_H_

#include "ukive/utils/json/json_structs.h"
#include "ukive/utils/string_utils.h"


// JSON 解析实现
// 参考 http://www.json.org/

namespace ukive {

    class JSONParser {
    public:
        using index_t = string8::size_type;
        using ValuePtr = std::shared_ptr<json::Value>;

        JSONParser();

        bool parse(const string8& json_str, ValuePtr* v);

    private:
        bool parseObject(const string8& json_str, index_t cur, index_t* next, json::ObjectValue** v);
        bool parseArray(const string8& json_str, index_t cur, index_t* next, json::ArrayValue** v);
        bool parseValue(const string8& json_str, index_t cur, index_t* next, json::Value** v);
        bool parseNumber(const string8& json_str, index_t cur, index_t* next, json::Value** v);
        bool parseString(const string8& json_str, index_t cur, index_t* next, std::string* val);
        void eatWhitespace(const string8& json_str, index_t cur, index_t* next);

        bool isDigit(char ch) const;
        bool isDigit1_9(char ch) const;
        bool isHexDigit(char ch) const;
        uint8_t getHexVal(char ch) const;
    };

}

#endif  // UKIVE_UTILS_JSON_JSON_PARSER_H_