#ifndef UTILS_JSON_JSON_STRUCTS_H_
#define UTILS_JSON_JSON_STRUCTS_H_

#include <map>
#include <vector>


namespace utl {
namespace json {

    class IntegerValue;
    class DoubleValue;
    class StringValue;
    class BoolValue;
    class NullValue;
    class ArrayValue;
    class ObjectValue;

    class Value {
    public:
        enum Type {
            OBJECT,
            ARRAY,
            INTEGER,
            DOUBLE,
            STRING,
            BOOL,
            NULL_VAL,
        };

        virtual ~Value() = default;

        virtual Type getType() const = 0;

        IntegerValue* asInteger() {
            if (getType() != INTEGER) { return nullptr; }
            return reinterpret_cast<IntegerValue*>(this);
        }
        DoubleValue* asDouble() {
            if (getType() != DOUBLE) { return nullptr; }
            return reinterpret_cast<DoubleValue*>(this);
        }
        StringValue* asString() {
            if (getType() != STRING) { return nullptr; }
            return reinterpret_cast<StringValue*>(this);
        }
        BoolValue* asBool() {
            if (getType() != BOOL) { return nullptr; }
            return reinterpret_cast<BoolValue*>(this);
        }
        NullValue* asNull() {
            if (getType() != NULL_VAL) { return nullptr; }
            return reinterpret_cast<NullValue*>(this);
        }
        ArrayValue* asArray() {
            if (getType() != ARRAY) { return nullptr; }
            return reinterpret_cast<ArrayValue*>(this);
        }
        ObjectValue* asObject() {
            if (getType() != OBJECT) { return nullptr; }
            return reinterpret_cast<ObjectValue*>(this);
        }
    };


    class IntegerValue : public Value {
    public:
        explicit IntegerValue(int64_t val);

        Type getType() const override { return INTEGER; }

        int64_t getValue() const { return val_; }

    private:
        int64_t val_;
    };


    class DoubleValue : public Value {
    public:
      explicit DoubleValue(double val);

      Type getType() const override { return DOUBLE; }

      double getValue() const { return val_; }

    private:
      double val_;
    };


    class StringValue : public Value {
    public:
        explicit StringValue(const std::string& val);

        Type getType() const override { return STRING; }

        void getValue(std::string* val) const { *val = val_; }

    private:
        std::string val_;
    };


    class BoolValue : public Value {
    public:
        explicit BoolValue(bool val);

        Type getType() const override { return BOOL; }

        bool getValue() const { return val_; }

    private:
        bool val_;
    };


    class NullValue : public Value {
    public:
        NullValue() = default;

        Type getType() const override { return NULL_VAL; }
    };


    class ArrayValue : public Value {
    public:
        ArrayValue();
        ~ArrayValue();

        Type getType() const override { return ARRAY; }

        size_t getCount() const;

        void put(Value* v);

        bool get(size_t index, Value** v) const;
        bool getInteger(size_t index, int64_t* out) const;
        bool getDouble(size_t index, double* out) const;
        bool getString(size_t index, std::string* out) const;
        bool getBoolean(size_t index, bool* out) const;
        bool getArray(size_t index, ArrayValue** out) const;
        bool getObject(size_t index, ObjectValue** out) const;

    private:
        std::vector<Value*> values_;
    };


    class ObjectValue : public Value {
    public:
        ObjectValue();
        ~ObjectValue();

        Type getType() const override { return OBJECT; }

        void put(const std::string& path, Value* v);

        bool get(const std::string& path, Value** out) const;
        bool getInteger(const std::string& path, int64_t* out) const;
        bool getDouble(const std::string& path, double* out) const;
        bool getString(const std::string& path, std::string* out) const;
        bool getBoolean(const std::string& path, bool* out) const;
        bool getArray(const std::string& path, ArrayValue** out) const;
        bool getObject(const std::string& path, ObjectValue** out) const;

    private:
        using ValMap = std::map<std::string, Value*>;

        bool getWne(const std::string& key, Value** out) const;

        ValMap map_;
    };

}
}

#endif  // UTILS_JSON_JSON_STRUCTS_H_