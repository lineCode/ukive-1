#ifndef SHELL_TEST_LIST_TEST_ADAPTER_H_
#define SHELL_TEST_LIST_TEST_ADAPTER_H_

#include <vector>

#include "ukive/views/list/list_adapter.h"
#include "ukive/utils/string_utils.h"
#include "ukive/utils/stl_utils.h"


namespace ukive {
    class TextView;
    class ImageView;
}

namespace shell {

    static const int ID_TITLE = 1;
    static const int ID_SUMMARY = 2;
    static const int ID_AVATAR = 3;

    class TestViewHolder : public ukive::ListAdapter::ViewHolder
    {
    public:
        TestViewHolder(ukive::View *v)
            :ViewHolder(v) {

            title_label = reinterpret_cast<ukive::TextView*>(v->findViewById(ID_TITLE));
            summary_label = reinterpret_cast<ukive::TextView*>(v->findViewById(ID_SUMMARY));
            avatar_image = reinterpret_cast<ukive::ImageView*>(v->findViewById(ID_AVATAR));
        }

        ukive::TextView *title_label;
        ukive::TextView *summary_label;
        ukive::ImageView *avatar_image;
    };

    class TestAdapter : public ukive::ListAdapter
    {
    public:
        ~TestAdapter() {
            ukive::STLDeleteElements(&data_list_);
        }

        struct BindData {
            int image_resource_id;
            ukive::string16 title;
            ukive::string16 summary;
        };

        ViewHolder* onCreateViewHolder(ukive::ViewGroup *parent, int position) override;
        void onBindViewHolder(ViewHolder *holder, int position) override;
        size_t getItemCount() override;

        void AddItem(int image_res_id, ukive::string16 title, ukive::string16 summary);
        void AddItem(size_t pos, int image_res_id, ukive::string16 title, ukive::string16 summary);
        void ModifyItem(int image_res_id, ukive::string16 title, ukive::string16 summary);
        void RemoveItem(ukive::string16 title);
        void RemoveItem(size_t pos);

    private:
        std::vector<BindData*> data_list_;
    };

}

#endif  // SHELL_TEST_LIST_TEST_ADAPTER_H_