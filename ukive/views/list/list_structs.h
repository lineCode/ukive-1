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
         * ����Ƿ��������е� View ������������
         * ����ǣ����� true�����򷵻� false��
         */
        bool isTopFilled(int dy) const;

        /**
         * ����Ƿ��������е� View �������ײ���
         * ����ǣ����� true�����򷵻� false��
         */
        bool isBottomFilled(int dy) const;

        /**
         * ����Ƿ��������һ���е� View ����������
         * ���ҵ�һ���ǳ����ģ��������ڿ�λ����
         * �÷����ٶ������ǹ����ġ�
         * ����ǣ����� true�����򷵻� false��
         */
        bool isTopFilled2(int dy) const;

        /**
         * ����Ƿ��������һ���е� View �����ײ���
         * �������һ���ǳ����ģ��������ڿ�λ����
         * �÷����ٶ������ǹ����ġ�
         * ����ǣ����� true�����򷵻� false��
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