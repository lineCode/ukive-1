#ifndef UKIVE_VIEWS_LIST_LIST_STRUCTS_H_
#define UKIVE_VIEWS_LIST_LIST_STRUCTS_H_

#include <vector>

#include "ukive/views/list/list_adapter.h"


namespace ukive {

    // Column
    class Column {
    public:
        Column();

        void setVertical(int top, int bottom);
        void setHorizontal(int left, int right);

        void addHolder(ListAdapter::ViewHolder* holder);
        void addHolder(ListAdapter::ViewHolder* holder, int index);
        void removeHolder(int index);
        void removeHolders(int start);
        void removeHolders(int start, int length);
        void clear();

        int getLeft() const;
        int getRight() const;
        int getWidth() const;
        int getHoldersTop() const;
        int getHoldersBottom() const;
        int getHolderCount() const;
        int getFinalScroll(int dy) const;
        int getIndexOfHolder(int adapter_pos) const;
        int getIndexOfFirstVisible(int dy) const;
        int getIndexOfLastVisible(int dy) const;
        ListAdapter::ViewHolder* getFront() const;
        ListAdapter::ViewHolder* getRear() const;
        ListAdapter::ViewHolder* getHolder(int index) const;
        ListAdapter::ViewHolder* getFirstVisible(int dy = 0) const;
        ListAdapter::ViewHolder* getLastVisible(int dy = 0) const;
        ListAdapter::ViewHolder* getHolderByPos(int adapter_pos) const;
        ListAdapter::ViewHolder* findAndInsertHolder(int start_index, int item_id);
        ListAdapter::ViewHolder* findHolderFromView(View* v) const;

        bool atTop() const;
        bool atBottom() const;
        bool isTopFilled(int dy) const;
        bool isBottomFilled(int dy) const;

    private:
        int top_, bottom_;
        int left_, right_;
        std::vector<ListAdapter::ViewHolder*> holders_;
    };


    // ColumnCollection
    class ColumnCollection {
    public:
        explicit ColumnCollection(int col_count);

        Column& operator[](int col);

        void setVertical(int top, int bottom);
        void setHorizontal(int left, int right);

        void clear();

        int getFinalScroll(int dy) const;
        ListAdapter::ViewHolder* getFirst() const;
        ListAdapter::ViewHolder* getLast() const;
        ListAdapter::ViewHolder* getTopStart() const;
        ListAdapter::ViewHolder* getBottomStart() const;
        ListAdapter::ViewHolder* getHolderByPos(int adapter_pos) const;
        ListAdapter::ViewHolder* getTopmost() const;
        ListAdapter::ViewHolder* getBottomost() const;
        ListAdapter::ViewHolder* findHolderFromView(View* v) const;

        bool isAllAtTop() const;
        bool isAllAtBottom() const;

        /**
         * 检测是否所有列中的 View 都超过顶部，
         * 如果是，返回 true，否则返回 false。
         */
        bool isTopFilled(int dy) const;

        /**
         * 检测是否所有列中的 View 都超过底部，
         * 如果是，返回 true，否则返回 false。
         */
        bool isBottomFilled(int dy) const;

        /**
         * 检测是否存在至少一列中的 View 超过顶部，
         * 并且第一行是充满的（即不存在空位）。
         * 该方法假定排列是规整的。
         * 如果是，返回 true，否则返回 false。
         */
        bool isTopFilled2(int dy) const;

        /**
         * 检测是否存在至少一列中的 View 超过底部，
         * 并且最后一行是充满的（即不存在空位）。
         * 该方法假定排列是规整的。
         * 如果是，返回 true，否则返回 false。
         */
        bool isBottomFilled2(int dy) const;

        bool isAllAtCeil(int item_count) const;
        bool isAllAtFloor(int item_count) const;

    private:
        int col_count_;
        std::vector<Column> columns_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_STRUCTS_H_