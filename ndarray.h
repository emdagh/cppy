#pragma once

#include <valarray>

namespace ndarray {


    template <typename T>
    auto row(std::valarray<T>& in, std::size_t i, std::size_t width) 
    -> std::valarray<T> {
        return std::valarray<T>(in[std::slice(i * width, width, 1)]);
    }

    template <typename T>
    auto column(std::valarray<T>& in, std::size_t i, std::size_t width) 
        -> std::valarray<T> {
        return std::valarray<T>(in[std::slice(i, in.size() / width, width)]);
    }


    size_t indexnd(const std::valarray<std::size_t>& dim, const std::valarray<std::size_t>& ix) {
        std::vector<size_t> fac;
        std::partial_sum(std::begin(dim), std::end(dim) - 1, std::back_inserter(fac),
                [&] (const size_t a, const size_t b) {
                    return a * b;
                });

        return std::inner_product(std::begin(ix), std::end(ix), std::begin(fac), 0);
    }

    

    template <typename T>
    inline T dot(const std::valarray<T>& a, const std::valarray<T>& b) {
        return (a * b).sum();
    }

    template <typename T>
    auto matmul(const std::valarray<T>& a, const std::valarray<T>& b, int m, int p, int q) 
    -> std::valarray<T>
    {
        std::valarray<T> ret(m * q);
        for(auto i=0; i < m; i++) {
            const auto& r = static_cast<std::valarray<T> >(a[std::slice(i * p, p, 1)]);
            for(auto j=0; j < q; j++) {
                const auto& c = static_cast<std::valarray<T> >(a[std::slice(j, p, q)]);
                ret[j + i * q] = ndarray::dot(c, r);//(c * r).sum();
            }
        }
        return ret;
    }


}
