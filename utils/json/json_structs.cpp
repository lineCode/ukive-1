#include "utils/json/json_structs.h"


namespace utl {
namespace json {

    IntegerValue::IntegerValue(int64_t val)
        : val_(val) {}

    DoubleValue::DoubleValue(double val)
        : val_(val) {}

    StringValue::StringValue(const std::string& val)
        : val_(val) {}

    BoolValue::BoolValue(bool val)
        : val_(val) {}

    ArrayValue::ArrayValue() {}

    ArrayValue::~ArrayValue() {
        for (auto v : values_) {
            delete v;
        }
    }

    size_t ArrayValue::getCount() const {
        return values_.size();
    }

    void ArrayValue::put(Value* v) {
        values_.push_back(v);
    }

    bool ArrayValue::get(size_t index, Value** v) const {
        if (index >= values_.size() || !v) {
            return false;
        }

        *v = values_[index];
        return true;
    }

    bool ArrayValue::getInteger(size_t index, int64_t* out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != INTEGER) {
            return false;
        }

        *out = val->asInteger()->getValue();
        return true;
    }

    bool ArrayValue::getDouble(size_t index, double* out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != DOUBLE) {
            return false;
        }

        *out = val->asDouble()->getValue();
        return true;
    }

    bool ArrayValue::getString(size_t index, std::string* out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != STRING) {
            return false;
        }

        val->asString()->getValue(out);
        return true;
    }

    bool ArrayValue::getBoolean(size_t index, bool* out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != BOOL) {
            return false;
        }

        *out = val->asBool()->getValue();
        return true;
    }

    bool ArrayValue::getArray(size_t index, ArrayValue** out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != ARRAY) {
            return false;
        }

        *out = val->asArray();
        return true;
    }

    bool ArrayValue::getObject(size_t index, ObjectValue** out) const {
        if (index >= values_.size() || !out) {
            return false;
        }

        auto val = values_[index];
        if (!val || val->getType() != OBJECT) {
            return false;
        }

        *out = val->asObject();
        return true;
    }

    ObjectValue::ObjectValue() {}

    ObjectValue::~ObjectValue() {
        for (auto pair : map_) {
            delete pair.second;
        }
    }

    void ObjectValue::put(const std::string& path, Value* v) {
        auto it = map_.find(path);
        if (it != map_.end() && it->second != v) {
            delete it->second;
        }
        map_[path] = v;
    }

    bool ObjectValue::get(const std::string& path, Value** out) const {
        if (!out) {
            return false;
        }

        size_t idx = 0;
        Value* val;
        const ObjectValue* obj_val = this;
        for (;;) {
            auto dot_idx = path.find(".", idx);
            if (dot_idx != std::string::npos) {
                auto cur_key = path.substr(idx, dot_idx - idx);
                idx = dot_idx + 1;
                if (!obj_val->getWne(cur_key, &val)) {
                    return false;
                }
                obj_val = val->asObject();
            } else {
                auto cur_key = path.substr(idx);
                if (!obj_val->getWne(cur_key, out)) {
                    return false;
                }
                break;
            }
        }
        return true;
    }

    bool ObjectValue::getInteger(const std::string& path, int64_t* out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != INTEGER) {
            return false;
        }
        *out = val->asInteger()->getValue();
        return true;
    }

    bool ObjectValue::getDouble(const std::string& path, double* out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != DOUBLE) {
            return false;
        }
        *out = val->asDouble()->getValue();
        return true;
    }

    bool ObjectValue::getString(const std::string& path, std::string* out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != STRING) {
            return false;
        }
        val->asString()->getValue(out);
        return true;
    }

    bool ObjectValue::getBoolean(const std::string& path, bool* out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != BOOL) {
            return false;
        }
        *out = val->asBool()->getValue();
        return true;
    }

    bool ObjectValue::getArray(const std::string& path, ArrayValue** out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != ARRAY) {
            return false;
        }
        *out = val->asArray();
        return true;
    }

    bool ObjectValue::getObject(const std::string& path, ObjectValue** out) const {
        Value* val;
        if (!get(path, &val) || !val || val->getType() != OBJECT) {
            return false;
        }
        *out = val->asObject();
        return true;
    }

    bool ObjectValue::getWne(const std::string& key, Value** out) const {
        auto it = map_.find(key);
        if (it == map_.end()) {
            return false;
        }

        *out = it->second;
        return true;
    }

}
}