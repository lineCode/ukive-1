#ifndef UKIVE_RESOURCES_LAYOUT_INSTANTIATOR_H_
#define UKIVE_RESOURCES_LAYOUT_INSTANTIATOR_H_

#include <functional>
#include <map>

#include "ukive/views/view.h"
#include "ukive/utils/string_utils.h"
#include "ukive/utils/xml/xml_structs.h"


namespace ukive {

    class View;
    class Window;

    class LayoutInstantiator {
    public:
        using Handler = std::function<View*(Window* w, const View::Attributes& attrs)>;
        using ElementPtr = std::shared_ptr<ukive::xml::Element>;

        LayoutInstantiator();

        static void init();

        View* instantiate(Window* w, int layout_id);

    private:
        bool fetchLayoutFileName(int layout_id, string16* file_name);
        bool traverseTree(const ElementPtr& element, View** parent);

        bool has_read_lim_;
        Window* host_win_;
        std::map<int, string16> layout_id_map_;

        static std::map<string16, Handler> handler_map_;
    };

}

#endif  // UKIVE_RESOURCES_LAYOUT_INSTANTIATOR_H_