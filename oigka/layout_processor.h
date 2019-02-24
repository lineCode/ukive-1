#ifndef OIGKA_LAYOUT_PROCESSOR_H_
#define OIGKA_LAYOUT_PROCESSOR_H_

#include <map>

#include "ukive/utils/string_utils.h"
#include "ukive/utils/xml/xml_structs.h"


namespace oigka {

    class LayoutProcessor {
    public:
        using IdMap = std::map<string8, int>;
        using ElementPtr = std::shared_ptr<ukive::xml::Element>;

        LayoutProcessor();

        bool process(const string16& res_dir, const string16& out_dir);

        const IdMap& getViewIdMap() const;
        const IdMap& getLayoutIdMap() const;

    private:
        bool traverseTree(const ElementPtr& element, bool is_first, IdMap* cur_map);

        bool need_second_;
        int cur_view_id_;
        int cur_layout_id_;
        IdMap view_id_map_;
        IdMap layout_id_map_;
    };

}

#endif  // OIGKA_LAYOUT_PROCESSOR_H_