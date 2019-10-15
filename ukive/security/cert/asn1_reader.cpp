#include "ukive/security/cert/asn1_reader.h"

#include "assert.h"

#define CHECK_RANGE() {  \
    if (!checkRange(0)) return false;  \
}


namespace ukive {
namespace cert {

    ASN1Reader::ASN1Reader(const std::string& payload)
        : data_(payload) {
    }

    ASN1Reader::~ASN1Reader() {
        assert(stack_.empty());
    }

    bool ASN1Reader::beginSequence() {
        TagClass tc;
        bool is_constructed, is_indef;
        uint64_t tag_num, length;
        if (!nextValue(&tc, &is_constructed, &tag_num, &length, &is_indef)) {
            return false;
        }

        if (tc != TagClass::Universal || !is_constructed) {
            return false;
        }

        auto tag = getUniversalTag(tag_num);
        if (tag != UniversalTags::SequenceAndOf) {
            return false;
        }

        pushBlock(length, is_indef);
        return true;
    }

    void ASN1Reader::endSequence() {
        popBlock();
    }

    bool ASN1Reader::beginSet() {
        TagClass tc;
        bool is_constructed, is_indef;
        uint64_t tag_num, length;
        if (!nextValue(&tc, &is_constructed, &tag_num, &length, &is_indef)) {
            return false;
        }

        if (tc != TagClass::Universal || !is_constructed) {
            return false;
        }

        auto tag = getUniversalTag(tag_num);
        if (tag != UniversalTags::SetAndOf) {
            return false;
        }

        pushBlock(length, is_indef);
        return true;
    }

    void ASN1Reader::endSet() {
        popBlock();
    }

    bool ASN1Reader::beginContextSpecific(uint64_t check_tag_num) {
        TagClass tc;
        bool is_constructed, is_indef;
        uint64_t tag_num, length;
        if (!nextValue(&tc, &is_constructed, &tag_num, &length, &is_indef)) {
            return false;
        }

        if (tc != TagClass::ContextSpecific || !is_constructed) {
            return false;
        }

        if (tag_num != check_tag_num) {
            return false;
        }

        pushBlock(length, is_indef);
        return true;
    }

    void ASN1Reader::endContextSpecific() {
        popBlock();
    }

    bool ASN1Reader::isOutOfBounds() const {
        return !checkRange(0);
    }

    bool ASN1Reader::nextValue() {
        TagClass tc;
        bool is_constructed, is_indef;
        uint64_t tag_num, length;
        return nextValue(&tc, &is_constructed, &tag_num, &length, &is_indef);
    }

    bool ASN1Reader::nextValue(
        TagClass* tc, bool* is_constructed, uint64_t* tag_num,
        uint64_t* length, bool* is_indef)
    {
        CHECK_RANGE();
        // Identifier octets
        uint8_t id_head = data_[index_++];

        *tc = TagClass(id_head >> 6);
        *is_constructed = (id_head & 0x20);
        bool has_more_octets = (id_head & 0x1F) == 0x1F;

        uint64_t tag_number = 0;
        if (has_more_octets) {
            uint8_t next;
            uint8_t count = 0;
            do {
                CHECK_RANGE();
                next = data_[index_++];
                tag_number <<= 7;
                tag_number |= next & 0x7F;

                ++count;
                assert(count < 10);
                if (count >= 10) {
                    return false;
                }
            } while (next & 0x80);
        } else {
            tag_number = id_head & 0x1F;
        }

        *tag_num = tag_number;

        CHECK_RANGE();
        // Length octets
        uint8_t length_head = data_[index_++];
        bool is_long_form = length_head & 0x80;
        bool is_indefinite_form = false;

        uint64_t len = 0;
        if (is_long_form) {
            uint8_t next_bytes = length_head & 0x7F;
            is_indefinite_form = next_bytes == 0;

            assert(next_bytes <= 8);
            if (next_bytes > 8) {
                return false;
            }

            for (uint8_t i = 0; i < next_bytes; ++i) {
                CHECK_RANGE();
                uint8_t next = data_[index_++];
                len <<= 8;
                len |= next;
            }
        } else {
            len = length_head & 0x7F;
        }

        *is_indef = is_indefinite_form;
        *length = len;
        return true;
    }

    bool ASN1Reader::getInteger(uint64_t* val) {
        TagClass tc;
        bool is_constructed, is_indef;
        uint64_t tag_num, length;
        if (!nextValue(&tc, &is_constructed, &tag_num, &length, &is_indef)) {
            return false;
        }

        if (tc != TagClass::Universal || is_constructed) {
            return false;
        }

        auto tag = getUniversalTag(tag_num);
        if (tag != UniversalTags::Integer) {
            return false;
        }

        uint64_t out = 0;
        if (is_indef) {
            uint8_t count = 0;
            bool prev = false;
            for (;;) {
                CHECK_RANGE();
                uint8_t cur = data_[index_++];
                if (prev && (cur == 0)) {
                    out >>= 8;
                    break;
                }
                out <<= 8;
                out |= cur;
                prev = (cur == 0);

                ++count;
                assert(count <= 8);
                if (count > 8) return false;
            }
        } else {
            assert(length <= 8);
            if (length > 8) return false;

            for (uint64_t i = 0; i < length; ++i) {
                out <<= 8;
                CHECK_RANGE();
                out |= data_[index_++];
            }
        }

        *val = out;
        return true;
    }

    bool ASN1Reader::getBigInteger(std::string* bytes) {
        TagClass tc;
        bool is_constructed, is_indef;
        uint64_t tag_num, length;
        if (!nextValue(&tc, &is_constructed, &tag_num, &length, &is_indef)) {
            return false;
        }

        if (tc != TagClass::Universal || is_constructed) {
            return false;
        }

        auto tag = getUniversalTag(tag_num);
        if (tag != UniversalTags::Integer) {
            return false;
        }

        return getContentBytes(length, is_indef, bytes);
    }

    bool ASN1Reader::getObjectID(ObjectID* obj_id) {
        TagClass tc;
        bool is_constructed, is_indef;
        uint64_t tag_num, length;
        if (!nextValue(&tc, &is_constructed, &tag_num, &length, &is_indef)) {
            return false;
        }

        if (tc != TagClass::Universal || is_constructed) {
            return false;
        }

        auto tag = getUniversalTag(tag_num);
        if (tag != UniversalTags::ObjectId) {
            return false;
        }

        uint8_t count = 0;
        uint64_t subid = 0;
        if (is_indef) {
            bool prev = false;
            for (;;) {
                CHECK_RANGE();
                uint8_t cur = data_[index_++];
                if (prev && (cur == 0)) {
                    subid >>= 8;
                    break;
                }
                subid <<= 7;
                subid |= cur & 0x7F;
                prev = (cur == 0);

                ++count;
                assert(count < 10);
                if (count >= 10) return false;

                if (!(cur & 0x80)) {
                    obj_id->push_back(subid);
                    subid = 0;
                    count = 0;
                }
            }
        } else {
            for (uint64_t i = 0; i < length; ++i) {
                subid <<= 7;
                CHECK_RANGE();
                uint8_t cur = data_[index_++];
                subid |= cur & 0x7F;

                ++count;
                assert(count < 10);
                if (count >= 10) return false;

                if (!(cur & 0x80)) {
                    obj_id->push_back(subid);
                    subid = 0;
                    count = 0;
                }
            }
        }

        return true;
    }

    bool ASN1Reader::getAny(std::string* any) {
        TagClass tc;
        bool is_constructed, is_indef;
        uint64_t tag_num, length;
        if (!nextValue(&tc, &is_constructed, &tag_num, &length, &is_indef)) {
            return false;
        }

        return getContentBytes(length, is_indef, any);
    }

    bool ASN1Reader::getOctetString(UniversalTags check_tag, std::string* str) {
        TagClass tc;
        bool is_constructed, is_indef;
        uint64_t tag_num, length;
        if (!nextValue(&tc, &is_constructed, &tag_num, &length, &is_indef)) {
            return false;
        }

        if (tc != TagClass::Universal) {
            return false;
        }

        auto tag = getUniversalTag(tag_num);
        if (tag != check_tag) {
            return false;
        }

        if (!is_constructed) {
            return getContentBytes(length, is_indef, str);
        }

        pushBlock(length, is_indef);

        while (!isOutOfBounds()) {
            if (is_indef) {
                if (data_[index_++] == 0) {
                    CHECK_RANGE();
                    if (data_[index_++] == 0) {
                        break;
                    }
                }
            }

            std::string tmp;
            if (!getOctetString(UniversalTags::OctetString, &tmp)) {
                return false;
            }
            str->append(tmp);
        }

        popBlock();
        return true;
    }

    bool ASN1Reader::getBitString(UniversalTags check_tag, std::string* str, uint8_t* unused) {
        TagClass tc;
        bool is_constructed, is_indef;
        uint64_t tag_num, length;
        if (!nextValue(&tc, &is_constructed, &tag_num, &length, &is_indef)) {
            return false;
        }

        if (tc != TagClass::Universal) {
            return false;
        }

        auto tag = getUniversalTag(tag_num);
        if (tag != check_tag) {
            return false;
        }

        if (!is_constructed) {
            CHECK_RANGE();
            *unused = data_[index_++];
            return getContentBytes(length, is_indef, str);
        }

        pushBlock(length, is_indef);

        while (!isOutOfBounds()) {
            if (is_indef) {
                if (data_[index_++] == 0) {
                    CHECK_RANGE();
                    if (data_[index_++] == 0) {
                        break;
                    }
                }
            }

            std::string tmp;
            if (!getBitString(UniversalTags::BitString, &tmp, unused)) {
                return false;
            }
            str->append(tmp);
        }

        popBlock();
        return true;
    }

    bool ASN1Reader::getTime(std::string* str) {
        TagClass tc;
        bool is_constructed, is_indef;
        uint64_t tag_num, length;
        if (!nextValue(&tc, &is_constructed, &tag_num, &length, &is_indef)) {
            return false;
        }

        if (tc != TagClass::Universal) {
            return false;
        }

        auto tag = getUniversalTag(tag_num);
        if (tag != UniversalTags::UTCTime && tag != UniversalTags::GeneralizedTime) {
            return false;
        }

        if (!is_constructed) {
            return getContentBytes(length, is_indef, str);
        }

        pushBlock(length, is_indef);

        while (!isOutOfBounds()) {
            if (is_indef) {
                if (data_[index_++] == 0) {
                    CHECK_RANGE();
                    if (data_[index_++] == 0) {
                        break;
                    }
                }
            }

            std::string tmp;
            if (!getOctetString(UniversalTags::OctetString, &tmp)) {
                return false;
            }
            str->append(tmp);
        }

        popBlock();
        return true;
    }

    bool ASN1Reader::getContentBytes(uint64_t length, bool is_indef, std::string* bytes) {
        if (is_indef) {
            bool prev = false;
            for (;;) {
                CHECK_RANGE();
                uint8_t cur = data_[index_++];
                if (prev && (cur == 0)) {
                    break;
                }
                bytes->push_back(cur);
                prev = (cur == 0);
            }
        } else {
            for (uint64_t i = 0; i < length; ++i) {
                CHECK_RANGE();
                bytes->push_back(data_[index_++]);
            }
        }
        return true;
    }

    void ASN1Reader::pushBlock(uint64_t length, bool is_indef) {
        Record rec;
        rec.is_indef = is_indef;
        rec.length = length;
        if (!is_indef) {
            rec.end = index_ + length;
        }
        stack_.push(std::move(rec));
    }

    void ASN1Reader::popBlock() {
        assert(!stack_.empty());
        if (!stack_.empty()) {
            stack_.pop();
        }
    }

    bool ASN1Reader::checkRange(uint64_t will_read_len) const {
        if (!stack_.empty()) {
            auto& top = stack_.top();
            if (!top.is_indef) {
                if (index_ + will_read_len >= top.end) {
                    return false;
                }
                return true;
            }
        }
        if (index_ + will_read_len >= data_.size()) {
            return false;
        }
        return true;
    }

    ASN1Reader::UniversalTags ASN1Reader::getUniversalTag(uint64_t val) const {
        if (val >= uint64_t(UniversalTags::ReservedForAddenda)) {
            return UniversalTags::ReservedForAddenda;
        }
        return UniversalTags(val);
    }

}
}