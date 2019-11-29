#ifndef SHELL_TEST_LIST_TEST_ADAPTER_H_
#define SHELL_TEST_LIST_TEST_ADAPTER_H_

#include <vector>

#include "utils/string_utils.h"
#include "utils/stl_utils.h"

#include "ukive/views/list/list_adapter.h"


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
        TestViewHolder(ukive::View* v)
            :ViewHolder(v) {

            title_label = reinterpret_cast<ukive::TextView*>(v->findViewById(ID_TITLE));
            summary_label = reinterpret_cast<ukive::TextView*>(v->findViewById(ID_SUMMARY));
            avatar_image = reinterpret_cast<ukive::ImageView*>(v->findViewById(ID_AVATAR));
        }

        ukive::TextView* title_label;
        ukive::TextView* summary_label;
        ukive::ImageView* avatar_image;
    };

    class TestAdapter : public ukive::ListAdapter
    {
    public:
        ~TestAdapter() {
            utl::STLDeleteElements(&data_list_);
        }

        struct BindData {
            int image_resource_id;
            string16 title;
            string16 summary;
        };

        ViewHolder* onCreateViewHolder(ukive::ViewGroup* parent, int position) override;
        void onBindViewHolder(ViewHolder* holder, int position) override;
        int getItemCount() override;

        void AddItem(int image_res_id, const string16& title, const string16& summary);
        void AddItem(int pos, int image_res_id, const string16& title, const string16& summary);
        void ModifyItem(int image_res_id, const string16& title, const string16& summary);
        void RemoveItem(const string16& title);
        void RemoveItem(int pos);

    private:
        std::vector<BindData*> data_list_;
    };

}

#endif  // SHELL_TEST_LIST_TEST_ADAPTER_H_