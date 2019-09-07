#ifndef UKIVE_UTILS_BIG_INTEGER_INT_ARRAY_H_
#define UKIVE_UTILS_BIG_INTEGER_INT_ARRAY_H_

#include <cstdint>


namespace ukive {

    class IntArray {
    public:
        // 数组单元的数据类型
        using Digit = uint32_t;

        // 双精度类型（以 Digit 为单精度）
        using Word = uint64_t;


        IntArray();
        explicit IntArray(int alloc);
        IntArray(const IntArray& rhs);
        IntArray(IntArray&& rhs) noexcept;
        ~IntArray();

        IntArray& operator=(const IntArray& rhs);
        IntArray& operator=(IntArray&& rhs) noexcept;

        void swap(IntArray* rhs);
        void grow(int size);
        void shrink();

        void zero();
        void abs();
        void inv();
        void destroy();

        bool isOdd() const;
        bool isZero() const;
        bool isMinus() const;

        Digit* buf_;
        int used_;
        int alloc_;
        bool is_minus_;
    };

}

#endif  // UKIVE_UTILS_BIG_INTEGER_INT_ARRAY_H_