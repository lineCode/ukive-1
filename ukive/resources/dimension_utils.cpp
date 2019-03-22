#include "ukive/resources/dimension_utils.h"

#include "ukive/log.h"

#include "oigka/layout_constants.h"


namespace ukive {

    bool resolveDimension(Window* w, const string16& dm, float* out) {
        if (endWith(dm, L"dp", false)) {
            float val = 0;
            if (stringToNumber(dm.substr(0, dm.length() - 2), &val)) {
                *out = w->dpToPx(val);
                return true;
            }
        } else if (endWith(dm, L"px", false)) {
            float val = 0;
            if (stringToNumber(dm.substr(0, dm.length() - 2), &val)) {
                *out = val;
                return true;
            }
        }
        return false;
    }

    int resolveAttrInt(
        View::AttrsRef attrs, const string16& key, int def_val)
    {
        auto it = attrs.find(key);
        if (it != attrs.end()) {
            int val = 0;
            if (stringToNumber(it->second, &val)) {
                return val;
            }
            LOG(Log::WARNING) << "Cannot resolve int attr " << key
                << ", val: " << it->second;
        }
        return def_val;
    }

    float resolveAttrFloat(
        View::AttrsRef attrs, const string16& key, float def_val)
    {
        auto it = attrs.find(key);
        if (it != attrs.end()) {
            float val = 0;
            if (stringToNumber(it->second, &val)) {
                return val;
            }
            LOG(Log::WARNING) << "Cannot resolve float attr " << key
                << ", val: " << it->second;
        }
        return def_val;
    }

    float resolveAttrDimension(
        Window* w, View::AttrsRef attrs, const string16& key, int def_val)
    {
        auto it = attrs.find(key);
        if (it != attrs.end()) {
            float val = 0;
            if (resolveDimension(w, it->second, &val)) {
                return val;
            }
            LOG(Log::WARNING) << "Cannot resolve dimension attr " << key
                << ", val: " << it->second;
        }
        return def_val;
    }

    bool resolveAttrBool(
        View::AttrsRef attrs, const string16& key, bool def_val)
    {
        auto it = attrs.find(key);
        if (it != attrs.end()) {
            float val = 0;
            if (isEqual(it->second, L"true", false)) {
                return true;
            }
            if (isEqual(it->second, L"false", false)) {
                return false;
            }
            LOG(Log::WARNING) << "Cannot resolve bool attr " << key
                << ", val: " << it->second;
        }
        return def_val;
    }

    string16 resolveAttrString(
        View::AttrsRef attrs, const string16& key, const string16& def_val)
    {
        auto it = attrs.find(key);
        if (it != attrs.end()) {
            return it->second;
        }
        return def_val;
    }

}
