#include "shell/test/utils/json_unit_test.h"

#include <fstream>

#include "utils/json/json_parser.h"
#include "utils/files/file.h"


namespace shell {
namespace test {

    void TEST_JSON() {
        std::fstream dau_file("D:\\test.json");
        if (dau_file) {
            std::string json_str;
            dau_file >> json_str;

            utl::JSONParser parser;
            utl::JSONParser::ValuePtr value;
            if (parser.parse(json_str, &value)) {
                auto object = value->asObject();
                utl::json::ArrayValue* list_val;
                if (object->getArray("result", &list_val)) {
                    for (size_t i = 0; i < list_val->getCount(); ++i) {
                        utl::json::ObjectValue* item_val;
                        if (!list_val->getObject(i, &item_val)) {
                            continue;
                        }
                    }
                }
            }
        }
    }

}
}
