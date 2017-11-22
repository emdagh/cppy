#include <cstdint>
#include <valarray>
#include "log.h"

typedef std::valarray<std::size_t> uivalarray;

template <typename T>
std::ostream& operator << (std::ostream& os, const std::valarray<T>& va) {
    for(auto it=std::begin(va); it != std::prev(std::end(va)); ++it) {
        os << *it << ",";
    }
    os << *(std::prev(std::end(va)));
    return os;
}

std::size_t indexnd(const uivalarray& shape, const uivalarray& ix) {
    DEBUG_METHOD();
    DEBUG_VALUE_OF(shape);
    DEBUG_VALUE_OF(ix);
    std::size_t ret;
    for(auto i=0; i < shape.size(); ++i) {
        ret = i==0 ? ix[i] : ix[i] * shape[i] + ret; 
    }
    return ret;
}

int main(int argc, char* argv[]) {

    DEBUG_METHOD();
    DEBUG_VALUE_OF(indexnd({4,4}, {3,3}));
    

    return 0;
}
