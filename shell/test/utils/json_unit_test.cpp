#include "shell/test/utils/json_unit_test.h"

#include <fstream>

#include "ukive/utils/json/json_parser.h"
#include "ukive/files/file.h"


namespace shell {
namespace test {

    void TEST_JSON() {
        /*ukive::File file(L"D:\\DAUVer");
        auto files = file.listFiles([](const string16& name, bool is_dir)->bool {
            return !is_dir;
        });*/

        std::fstream dau_file("D:\\DAUVer\\20180903.json");
        if (dau_file) {
            std::string json_str;
            dau_file >> json_str;

            ukive::JSONParser parser;
            ukive::JSONParser::ValuePtr value;
            if (parser.parse(json_str, &value)) {
                auto object = value->asObject();
                ukive::json::ArrayValue* list_val;
                if (object->getArray("result", &list_val)) {
                    for (size_t i = 0; i < list_val->getCount(); ++i) {
                        ukive::json::ObjectValue* item_val;
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
