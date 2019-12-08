#include "ukive/net/tls_common.hpp"


namespace ukive {
namespace net {
namespace tls {

    stringu8 getUInt16Bytes(uint16_t val) {
        stringu8 ret;
        ret.push_back(val >> 8);
        ret.push_back(val & 0xFF);
        return ret;
    }

    stringu8 getUInt24Bytes(uint32_t val) {
        stringu8 ret;
        ret.push_back((val >> 16) & 0xFF);
        ret.push_back((val >> 8) & 0xFF);
        ret.push_back(val & 0xFF);
        return ret;
    }

    stringu8 getUInt32Bytes(uint32_t val) {
        stringu8 ret;
        ret.push_back(val >> 24);
        ret.push_back((val >> 16) & 0xFF);
        ret.push_back((val >> 8) & 0xFF);
        ret.push_back(val & 0xFF);
        return ret;
    }

    stringu8 getUInt64Bytes(uint64_t val) {
        stringu8 ret;
        ret.push_back(val >> 56);
        ret.push_back((val >> 48) & 0xFF);
        ret.push_back((val >> 40) & 0xFF);
        ret.push_back((val >> 32) & 0xFF);
        ret.push_back((val >> 24) & 0xFF);
        ret.push_back((val >> 16) & 0xFF);
        ret.push_back((val >> 8) & 0xFF);
        ret.push_back(val & 0xFF);
        return ret;
    }

}
}
}