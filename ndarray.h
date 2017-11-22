#pragma once

#include <valarray>

namespace ndarray {


    template <typename T>
    auto row(std::valarray<T>& in, std::size_t i, std::size_t width) 
    -> std::slice_array<T> {
        return in[std::slice(i * width, width, 1)];
    }

    template <typename T>
    auto column(std::valarray<T>& in, std::size_t i, std::size_t width) 
        -> std::slice_array<T> {
        return in[std::slice(i, in.size() / width, width)];
    }


    std::size_t indexnd(const std::valarray<std::size_t>& dim, const std::valarray<std::size_t>& ix) {
        assert(dim.size() == ix.size());
        // nd + Nd * (nd-1 + Nd-1 * (nd-2 + Nd-2 * (nd-3 ... ))) 
#if 0 
        std::size_t ii = 0;
        std::size_t ml = 1;
     
        for(auto i: range(dim.size())) {
            assert(ix[i] < dim[i]);
            ii += ix[i] * ml; 
            ml *= dim[i];
        }
        return ii;
#else
        std::vector<std::size_t> muls = { 1 };
        std::partial_sum(std::begin(dim), std::end(dim) - 1, std::back_inserter(muls), 
                [&] (const std::size_t& a, const std::size_t& b) {
                    return a * b;
                });
        return std::inner_product(begin(ix), std::end(ix), std::begin(muls), 0);
#endif
    }


    

    template <typename T>
    inline T dot(const std::valarray<T>& a, const std::valarray<T>& b) {
        return (a * b).sum();
    }

    // @param - m rows of a
    // @param - p cols of a <-> rows of b
    // @param - q cols of b
    // @returns p * q matrix
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

    template <typename T>
    std::valarray<T> colsum(std::valarray<T>& arr, unsigned dim) {
        std::valarray<T> res(dim);
        for(unsigned i=0; i < dim; i++) {
            res[i] = static_cast<std::valarray<T> >(col(arr, i, dim)).sum(); 
        }
        return res;
    }

    template <typename T>
    std::valarray<T> transposed(std::valarray<T>& arr, unsigned dim) {
        std::valarray<T> res(arr.size());
        unsigned rows = arr.size() / dim;
        for(unsigned i=0; i < rows; i++) {
            col(res, i, rows) = row(arr, i, dim);
        }
        return res;
    }

}
