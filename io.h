#pragma once

#include <iostream>

namespace io {

    template <typename T>
    void binary_read(std::istream& in, T& out) {
        in.read(reinterpret_cast<char*>(&out), sizeof(T));
    }

    template <typename T>
    void binary_write(std::ostream& out, const T& in) {
        out.write(reinterpret_cast<const char*>(&in), sizeof(T));
    }
}
