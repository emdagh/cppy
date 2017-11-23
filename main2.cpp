#include <cstdint>
#include <valarray>
#include "log.h"
#include "range.h"
#include <prettyprint.hpp>
typedef std::valarray<std::size_t> uivalarray;

template <typename T, size_t N>
class ndarray : public std::valarray<T> {
    std::array<int, N> _shape;

public:

    size_t size(int d) {
        return _shape[d];
    }
    
};

template <typename T>
std::ostream& operator << (std::ostream& os, const std::valarray<T>& va) {
    for(auto it=std::begin(va); it != std::prev(std::end(va)); ++it) {
        os << *it << ",";
    }
    os << *(std::prev(std::end(va)));
    return os;
}
namespace cpp {
template <typename T>
std::vector<T> reverse_range(T beg, T end) {
    auto n = end - beg; //std::distance(beg, end);
    std::vector<T> ret(n);
    std::iota(ret.rbegin(), ret.rend(), beg);
    return ret;
}
}

template <int F, int L>
struct static_for {
    template <typename Q>
    void operator () (const Q& fn) const {
        if(F < L) {
            fn(F);
            static_for<F + 1, L>()(fn);
        }
    }
};

template <int N>
struct static_for<N, N> {
    template <typename Q>
    void operator () (const Q& fn) const {
    }
};


/**
 * n-dimensional -> 1d indexer
 *
 * nd + Nd * (nd-1 + Nd-1 * (nd-2 + Nd-2 * (nd-3 ... )))
 * see also: https://eli.thegreenplace.net/2015/memory-layout-of-multi-dimensional-arrays/
 *
 * nd + Nd * (nd-1)
 * nd + Nd * (nd-1 + Nd-1 * (nd-2))
 */
template <typename T, size_t N>
std::size_t indexnd(const std::array<T, N>& shape, const std::array<T, N>& ix) {
    size_t ret = ix[N-1];
#if LOOPSTATIC

    static_for<0, N+1>()([&] (int j) {
        auto i = N - j;
        ret = std::fma(ret, shape[i], ix[i]);////ix[i] + shape[i] * ret;
    });
#else
    for(int i=N; i >= 0; --i) { 
        ret = std::fma(ret, shape[i], ix[i]);////ix[i] + shape[i] * ret;
    }
#endif
    return ret;
}

template <size_t N>
using size = std::integral_constant<size_t, N>;

template <typename T, size_t N>
class counter : public std::array<T, N> {
    using A = std::array<T, N>;
    A b, e;

    template <size_t I=0>
    void inc(size<I> = size<I>()) {
        if(++_<I>() != std::get<I>(e))
            return;

        _<I>() = std::get<I>(b);
        inc(size<I+1>());
    }

    void inc(size<N-1>) { ++_<N-1>(); }

public:
    counter(const A& b, const A& e) : A(b), b(b), e(e) {
    }

    counter& operator ++ (void) { return inc(), *this; }
    operator bool() const { return _<N-1>() != std::get<N-1>(e); }

    template <size_t I>
    T& _() { return std::get<I>(*this); }

    template <size_t I>
    constexpr const T& _() const { return std::get<I>(*this); }
            
};



int main(int argc, char* argv[]) {
    DEBUG_METHOD();
    constexpr int N = 3;
    std::array<int, N> ia = { 0, 0, 0 };
    std::array<int, N> ib = { 3, 3, 1 };

    std::valarray<float> a(std::accumulate(std::begin(ib), std::end(ib), 1, std::multiplies<int>()));
    std::iota(std::begin(a), std::end(a), 0.0f);

    //a *= 10;

    DEBUG_VALUE_OF(a);

    for(counter<int, N> c(ia, ib); c; ++c) {
        //std::array<int, N> xi = { c._<2>(), c._<1>(), c._<0>() };
        //DEBUG_VALUE_AND_TYPE_OF(xi);
        DEBUG_VALUE_AND_TYPE_OF(a[indexnd(ib, c)]);
        //DEBUG_VALUE_OF(c);
        //DEBUG_VALUE_OF(a[indexnd(ib, xi)]);

    }
    

    return 0;
}
