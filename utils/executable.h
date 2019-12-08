#ifndef UTILS_EXECUTABLE_H_
#define UTILS_EXECUTABLE_H_


namespace utl {

    class Executable {
    public:
        virtual ~Executable() = default;

        virtual void run() = 0;
    };

}

#endif  // UTILS_EXECUTABLE_H_