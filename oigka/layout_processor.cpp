#include "oigka/layout_processor.h"

#include <fstream>

#include "utils/log.h"
#include "utils/xml/xml_parser.h"
#include "utils/xml/xml_writer.h"
#include "utils/files/file.h"
#include "utils/stl_utils.h"

#include "oigka/layout_constants.h"


namespace oigka {

    LayoutProcessor::LayoutProcessor()
        : need_second_(false),
          cur_view_id_(10000),
          cur_layout_id_(10000) {}

    bool LayoutProcessor::process(const string16& res_dir, const string16& out_dir) {
        utl::File layout_dir(res_dir);
        auto xml_files = layout_dir.listFiles([](const string16& name, bool is_dir)->bool {
            return !is_dir && utl::endWith(name, L".xml", false);
        });

        if (xml_files.empty()) {
            LOG(Log::INFO) << "Cannot find xml files";
        } else {
            LOG(Log::INFO) << xml_files.size() << " xml file(s) found";
        }

        for (const auto& xml_file : xml_files) {
            LOG(Log::INFO) << "Processing xml file: " << xml_file.getPath();

            std::ifstream reader(xml_file.getPath(), std::ios::binary);
            if (reader.fail()) {
                LOG(Log::ERR) << "Cannot open xml file: " << xml_file.getName();
                return false;
            }

            auto cpos = reader.tellg();
            reader.seekg(0, std::ios_base::end);
            auto charSize = reader.tellg();
            reader.seekg(cpos);

            std::unique_ptr<char[]> buf(new char[utl::STLCU32(charSize)]());
            reader.read(buf.get(), charSize);

            string8 str(buf.get(), utl::STLCU32(charSize));

            utl::XMLParser xml_parser;
            std::shared_ptr<utl::xml::Element> root;
            if (!xml_parser.parse(str, &root)) {
                auto& pedometer = xml_parser.getPedometer();
                LOG(Log::ERR) << "Failed to parse xml file: " << xml_file.getName()
                    << " line: " << pedometer.getCurRow() << " col: " << pedometer.getCurCol();
                return false;
            }

            IdMap cur_map;
            if (!traverseTree(root, true, &cur_map)) {
                LOG(Log::ERR) << "Failed to traverse xml file: " << xml_file.getName();
                return false;
            }

            if (need_second_) {
                if (!traverseTree(root, false, &cur_map)) {
                    LOG(Log::ERR) << "Failed to traverse xml file: " << xml_file.getName();
                    return false;
                }
            }

            for (const auto& cur : cur_map) {
                view_id_map_.insert(cur);
            }
            cur_map.clear();

            string8 xml_str;
            utl::XMLWriter xml_writer;
            if (!xml_writer.write(*root, &xml_str)) {
                LOG(Log::ERR) << "Failed to write xml file: " << xml_file.getName();
                return false;
            }

            utl::File new_file(out_dir, xml_file.getName());
            if (!new_file.mkDirs(true)) {
                LOG(Log::ERR) << "Failed to make dir: " << new_file.getParentPath();
                return false;
            }
            std::ofstream writer(new_file.getPath(), std::ios::binary | std::ios::ate);
            if (writer.fail()) {
                LOG(Log::ERR) << "Cannot open file: " << new_file.getPath();
                return false;
            }

            writer.write(xml_str.data(), xml_str.length());

            auto xml_file_name = utl::UTF16ToUTF8(xml_file.getName());
            if (!xml_file_name.empty()) {
                layout_id_map_[xml_file_name] = cur_layout_id_;
                ++cur_layout_id_;
            }
        }

        LOG(Log::INFO) << "Generating layout id file...";

        if (!layout_id_map_.empty()) {
            string8 out_map_str;
            for (const auto& pair : layout_id_map_) {
                out_map_str.append(std::to_string(pair.second))
                    .append("=").append(pair.first).append("\n");
            }
            utl::File new_file(out_dir, kLayoutIdFileName);
            std::ofstream writer(new_file.getPath(), std::ios::binary | std::ios::ate);
            writer.write(out_map_str.data(), out_map_str.length());
        } else {
            LOG(Log::INFO) << "Layout id map is empty, no file generated";
        }

        return true;
    }

    const LayoutProcessor::IdMap& LayoutProcessor::getViewIdMap() const {
        return view_id_map_;
    }

    const LayoutProcessor::IdMap& LayoutProcessor::getLayoutIdMap() const {
        return layout_id_map_;
    }

    bool LayoutProcessor::traverseTree(const ElementPtr& element, bool is_first, IdMap* cur_map) {
        if (!element) {
            return false;
        }

        for (auto& attr : element->attrs) {
            auto attr_val = attr.second;
            if (utl::startWith(attr_val, "@")) {
                if (utl::startWith(attr_val, "@id/")) {
                    attr_val = attr_val.substr(4);
                    if (attr_val.empty()) {
                        LOG(Log::ERR) << "The id attr: " << utl::UTF8ToUTF16(attr.first)
                            << " of element: " << utl::UTF8ToUTF16(element->tag_name)
                            << " is invalid.";
                        return false;
                    }

                    auto it = cur_map->find(attr_val);
                    if (it == cur_map->end()) {
                        if (is_first) {
                            need_second_ = true;
                        } else {
                            LOG(Log::ERR) << "Cannot find id: " << utl::UTF8ToUTF16(attr_val)
                                << " of element: " << utl::UTF8ToUTF16(element->tag_name);
                            return false;
                        }
                    } else {
                        attr.second = std::to_string(it->second);
                    }
                } else if (utl::startWith(attr_val, "@+id/")) {
                    attr_val = attr_val.substr(5);
                    if (attr_val.empty()) {
                        LOG(Log::ERR) << "The id attr: " << utl::UTF8ToUTF16(attr.first)
                            << " of element: " << utl::UTF8ToUTF16(element->tag_name)
                            << " is invalid";
                        return false;
                    }

                    if (cur_map->find(attr_val) != cur_map->end()) {
                        LOG(Log::ERR) << "The id: " << utl::UTF8ToUTF16(attr_val)
                            << " of element: " << utl::UTF8ToUTF16(element->tag_name)
                            << " is duplicated";
                        return false;
                    }

                    cur_map->insert({ attr_val, cur_view_id_ });
                    attr.second = std::to_string(cur_view_id_);
                    ++cur_view_id_;
                } else {
                    LOG(Log::ERR) << "Unsupported @ operation: " << utl::UTF8ToUTF16(attr_val)
                        << " of element: " << utl::UTF8ToUTF16(element->tag_name);
                    return false;
                }
            }
        }

        for (const auto& content : element->contents) {
            if (content.type != utl::xml::Content::Type::Element) {
                continue;
            }
            if (!traverseTree(content.element, is_first, cur_map)) {
                return false;
            }
        }

        return true;
    }

}
