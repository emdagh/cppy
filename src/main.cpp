#include "ndarray.h"
#include "numeric.h"
#include "parallel.h"
#include "io.h"
#include "map.h"
#include "zip.h"
#include "tcp.h"
#include "aligned_allocator.h"
#include "range.h"
#include "log.h"

template <typename S, typename T>
S& operator << (S& os, const std::valarray<T>& in) {
    //std::copy(std::begin(in), std::end(in), std::ostream_iterator<T>(os));
    auto first = std::begin(in);
    for(; first != std::end(in) - 1; ++first) {
        os << *first << ",";
    }
    os << *std::prev(std::end(in));
    return os;
}



int main(int argc, char* argv[]) {
    DEBUG_METHOD();
    typedef std::valarray<float> tensor_type;

    tensor_type t = { 
        0.0f, 1.0f, 2.0f,
        3.0f, 4.0f, 5.0f,
        6.0f, 7.0f, 8.0f
    };

    int a = 1;
    int b = 2;


    DEBUG_VALUE_AND_TYPE_OF(a + b);
    DEBUG_VALUE_AND_TYPE_OF(t);

    DEBUG_VALUE_AND_TYPE_OF(ndarray::matmul(t, t, 3, 3, 3));

    return 0;
}

