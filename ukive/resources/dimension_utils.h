#ifndef UKIVE_RESOURCES_DIMENSION_UTILS_H_
#define UKIVE_RESOURCES_DIMENSION_UTILS_H_

#include "utils/string_utils.h"

#include "ukive/window/window.h"


namespace ukive {

    bool resolveDimension(Window* w, const string16& dm, float* out);

    int resolveAttrInt(
        View::AttrsRef attrs, const string16& key, int def_val);
    float resolveAttrFloat(
        View::AttrsRef attrs, const string16& key, float def_val);
    float resolveAttrDimension(
        Window* w, View::AttrsRef attrs, const string16& key, int def_val);
    bool resolveAttrBool(
        View::AttrsRef attrs, const string16& key, bool def_val);
    string16 resolveAttrString(
        View::AttrsRef attrs, const string16& key, const string16& def_val);

}

#endif  // UKIVE_RESOURCES_DIMENSION_UTILS_H_