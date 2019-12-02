#include "ukive/resources/layout_instantiator.h"

#include <fstream>

#include "oigka/layout_constants.h"

#include "utils/log.h"
#include "utils/xml/xml_parser.h"
#include "utils/files/file_utils.h"

#include "ukive/application.h"
#include "ukive/views/layout/view_group.h"

#include "ukive/views/button.h"
#include "ukive/views/chart_view.h"
#include "ukive/views/check_box.h"
#include "ukive/views/image_view.h"
#include "ukive/views/scroll_view.h"
#include "ukive/views/seek_bar.h"
#include "ukive/views/spinner_view/spinner_view.h"
#include "ukive/views/switch_view.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/layout/frame_layout.h"
#include "ukive/views/layout/linear_layout.h"
#include "ukive/views/layout/non_client_layout.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/root_layout.h"


#define VIEW_CONSTRUCTOR(name)  \
    handler_map_[L#name] = [](Window* w, const View::Attributes& attrs)->View* {  \
        return new name(w, attrs);  \
    };


namespace ukive {

    std::map<string16, LayoutInstantiator::Handler> LayoutInstantiator::handler_map_;

    LayoutInstantiator::LayoutInstantiator()
        : has_read_lim_(false),
          host_win_(nullptr) {}

    void LayoutInstantiator::init() {
        VIEW_CONSTRUCTOR(View);
        VIEW_CONSTRUCTOR(Button);
        VIEW_CONSTRUCTOR(ChartView);
        VIEW_CONSTRUCTOR(CheckBox);
        VIEW_CONSTRUCTOR(ImageView);
        VIEW_CONSTRUCTOR(SeekBar);
        VIEW_CONSTRUCTOR(SpinnerView);
        VIEW_CONSTRUCTOR(SwitchView);
        VIEW_CONSTRUCTOR(TextView);

        VIEW_CONSTRUCTOR(ViewGroup);
        VIEW_CONSTRUCTOR(FrameLayout);
        VIEW_CONSTRUCTOR(LinearLayout);
        VIEW_CONSTRUCTOR(ListView);
        VIEW_CONSTRUCTOR(NonClientLayout);
        VIEW_CONSTRUCTOR(RestraintLayout);
        VIEW_CONSTRUCTOR(RootLayout);
        VIEW_CONSTRUCTOR(ScrollView);
    }

    View* LayoutInstantiator::instantiate(Window* w, int layout_id) {
        DCHECK(w != nullptr);
        host_win_ = w;

        string16 xml_file_path;
        if (!fetchLayoutFileName(layout_id, &xml_file_path)) {
            return nullptr;
        }

        std::ifstream reader(xml_file_path, std::ios::binary);
        if (reader.fail()) {
            DCHECK(false);
            return nullptr;
        }

        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        auto charSize = reader.tellg();
        reader.seekg(cpos);

        std::unique_ptr<char[]> buf(new char[charSize]());
        reader.read(buf.get(), charSize);

        string8 str(buf.get(), charSize);

        utl::XMLParser parser;
        std::shared_ptr<utl::XMLParser::Element> root;
        if (!parser.parse(str, &root)) {
            DCHECK(false);
            return nullptr;
        }

        View* root_view = nullptr;
        if (!traverseTree(root, &root_view)) {
            DCHECK(false);
            return nullptr;
        }

        return root_view;
    }

    bool LayoutInstantiator::fetchLayoutFileName(int layout_id, string16* file_name) {
        DCHECK(file_name != nullptr);

        if (!has_read_lim_) {
            has_read_lim_ = true;

            auto file_path = utl::getExecFileName(true);
            file_path.append(L"\\oigka\\");
            auto lm_file_path = file_path + oigka::kLayoutIdFileName;

            std::ifstream id_file_reader(lm_file_path, std::ios::binary);
            if (id_file_reader.fail()) {
                DCHECK(false);
                return false;
            }

            auto cpos = id_file_reader.tellg();
            id_file_reader.seekg(0, std::ios_base::end);
            auto charSize = id_file_reader.tellg();
            id_file_reader.seekg(cpos);

            std::unique_ptr<char[]> buf(new char[charSize]());
            id_file_reader.read(buf.get(), charSize);

            string8 str(buf.get(), charSize);

            std::vector<string8> lines;
            string8::size_type prev_i = 0;
            for (string8::size_type i = 0; i < str.length(); ++i) {
                if (str[i] == '\r') {
                    lines.push_back(str.substr(prev_i, i - prev_i));
                    if (i + 1 < str.length() && str[i + 1] == '\n') {
                        i += 2;
                    } else {
                        ++i;
                    }
                    prev_i = i;
                } else if (str[i] == '\n') {
                    lines.push_back(str.substr(prev_i, i - prev_i));
                    ++i;
                    prev_i = i;
                }
            }

            for (const auto& line : lines) {
                if (line.empty()) {
                    continue;
                }

                auto pair = utl::splitString(line, "=");
                if (pair.size() != 2) {
                    continue;
                }

                uint32_t id;
                if (!utl::stringToNumber(pair[0], &id)) {
                    DCHECK(false);
                    return false;
                }

                layout_id_map_[id] = file_path + utl::UTF8ToUTF16(pair[1]);
            }
        }

        auto it = layout_id_map_.find(layout_id);
        if (it == layout_id_map_.end()) {
            return false;
        }

        *file_name = it->second;
        return true;
    }

    bool LayoutInstantiator::traverseTree(const ElementPtr& element, View** parent) {
        DCHECK(parent != nullptr);

        if (!element) {
            return false;
        }

        if (*parent && !(*parent)->isViewGroup()) {
            LOG(Log::ERR) << "The parent of the View: " << utl::UTF8ToUTF16(element->tag_name) << " is not a ViewGroup.";
            return false;
        }

        View::Attributes view_attrs;
        View::Attributes layout_attrs;
        for (auto& attr : element->attrs) {
            if (utl::startWith(attr.first, "layout_")) {
                layout_attrs[utl::UTF8ToUTF16(attr.first)] = utl::UTF8ToUTF16(attr.second);
            } else {
                view_attrs[utl::UTF8ToUTF16(attr.first)] = utl::UTF8ToUTF16(attr.second);
            }
        }

        auto it = handler_map_.find(utl::UTF8ToUTF16(element->tag_name));
        if (it == handler_map_.end()) {
            LOG(Log::ERR) << "Cannot find View: " << utl::UTF8ToUTF16(element->tag_name);
            return false;
        }

        auto view_constructor = it->second;
        auto cur_view = view_constructor(host_win_, view_attrs);
        if (!*parent) {
            *parent = cur_view;
        } else {
            auto view_group = static_cast<ViewGroup*>(*parent);
            auto lp = view_group->generateLayoutParamsByAttrs(layout_attrs);
            view_group->addView(cur_view, lp);
        }

        for (const auto& content : element->contents) {
            if (content.type != utl::xml::Content::Type::Element) {
                continue;
            }
            if (!traverseTree(content.element, &cur_view)) {
                return false;
            }
        }

        return true;
    }

}