//#include <tiny_dnn/tiny_dnn.h>
#include <datetime.h>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <cmath>
#define _DEBUG
#include <log.h>

#include <algorithm>
#include "aligned_allocator.h"
#include "prettyprint.hpp"

#include <iostream>
#include <iterator>
#include <vector>
#include <list>

#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include "range.hpp"
#include "zip.hpp"
#include "split.hpp"

#include "tensor.hpp"
#define argmax(x)   std::max_element(((x)).begin(), ((x)).end())


template <typename T>
std::slice_array<T> row(std::valarray<T>& arr, unsigned r, unsigned dim) {
    return arr[std::slice(r * dim, dim, 1)];
}

template <typename T>
std::slice_array<T> col(std::valarray<T>& arr, unsigned c, unsigned dim) {
    unsigned rows = arr.size() / dim;
    return arr[std::slice(c, rows, dim)];
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

template <typename T>
void identity(std::valarray<T>& a, unsigned dim) {
    std::fill(std::begin(a), std::end(a), 0.0f);
    a[std::slice(0, a.size(), dim + 1)] = 1.0f; 
}

template <typename T>
T trace(const std::valarray<T>& a, unsigned dim) {
    return a[std::gslice(0, {dim}, {dim+1})].sum();
}

template <typename T>
std::ostream& operator << (std::ostream& os, const std::valarray<T>& a) {
    std::copy(std::begin(a), std::end(a) - 1, std::ostream_iterator<float>(os, ","));
    os << *(std::end(a) - 1);
    return os;
}

template<template<typename T> class U, typename T>
std::ostream& operator << (std::ostream& out, const U<T>& x)
{
    out << static_cast<std::valarray<T> >(x);
    return out;
}


template <typename T, std::size_t R>
class ndarray {
    std::valarray<T> data;
    std::valarray<std::size_t> dim;
public:
    typedef std::valarray<std::size_t> index_type;
    ndarray(const index_type& dims) {
        DEBUG_METHOD();
        auto data_size = std::accumulate(std::begin(dims), std::end(dims), 1, std::multiplies<T>());
        DEBUG_VALUE_OF(data_size);
        data = std::valarray<T>(data_size);
        dim = dims;
        std::iota(std::begin(data), std::end(data), 0.0f);
        DEBUG_VALUE_OF(data);
    }

    std::size_t compute_index(const index_type& ix) const {
        DEBUG_METHOD();
        DEBUG_VALUE_OF(ix);

        assert(R == ix.size());
        
        std::size_t ii = 0;
        std::size_t ml = 1;

        for(auto i: range(dim.size())) {
            assert(ix[i] < dim[i]);
            ii += ix[i] * ml; 
            DEBUG_VALUE_OF(ii);
            ml *= dim[i];
        }
        // nd + Nd * (nd-1 + Nd-1 * (nd-2 + Nd-2)
        return ii;
    }
};

template <typename T>
std::valarray<T> tensordot(const std::valarray<T>& a,
        const std::valarray<T>& b,
        const std::valarray<std::size_t>& dim_a,
        const std::valarray<std::size_t>& dim_b)
{

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

#include <fstream>
#include "csv.hpp"
#include "matlab.hpp"

template <typename T>
void print_asmatrix(std::valarray<T>& a, unsigned dim) {
    DEBUG_METHOD();
    unsigned rb = a.size() / dim;
    for(auto i : range(rb)) {
        const auto& row = a[std::slice(i * dim, dim, 1)];
        DEBUG_VALUE_OF(row);
    }
}

// @param - m rows of a
// @param - p cols of a <-> rows of b
// @param - q cols of b
// @returns p * q matrix
template <typename T>
std::valarray<T> matmul(std::valarray<T>& a, std::valarray<T>& b, unsigned m, unsigned p, unsigned q) {
    DEBUG_METHOD();
    std::valarray<T> c(m * q);
    for(auto i=0; i < m; i++) {
        const auto& yy = static_cast<std::valarray<T> >(a[std::slice(i * p, p, 1)]);
        for(auto j=0; j < q; j++) {
            const auto& xx = static_cast<std::valarray<T> >(b[std::slice(j, p, q)]);
            c[j + i * q] = (xx * yy).sum();
        }
    }
    return c;
}

template <typename T>
T mean(const std::valarray<T>& a) {
    return a.sum() / a.size();
}


int main(int argc, char* argv[]) {
    DEBUG_METHOD();

    std::valarray<std::size_t> v = { 4, 4, 1 };
    DEBUG_VALUE_OF(indexnd(v, {3,3,0}));

    std::valarray<float> a(60);
    std::valarray<std::size_t> dim_a = { 3, 4, 5 };
    std::iota(std::begin(a), std::end(a), 0.0f);

    std::valarray<float> b(24);
    std::valarray<std::size_t> dim_b = { 4, 3, 2 };
    std::iota(std::begin(b), std::end(b), 0.0f);

    std::valarray<float> d(10);
    std::valarray<std::size_t> dim_d = { 5, 2 };
    std::fill(std::begin(d), std::end(d), 0.0f);

    for(auto i: range(5)) {
        for(auto j: range(2)) {
            for(auto k: range(3)) {
                for(auto n: range(4)) {
                    float x = a[indexnd({dim_a}, {k, n, i})];
                    float y = b[indexnd({dim_b}, {n, k, j})];
                    DEBUG_VALUE_OF(std::vector<int>({k, n, i, (int)x}));
                    DEBUG_VALUE_OF(std::vector<int>({n, k, j, (int)y}));

                    d[i + j * 2] += x * y;
                    //DEBUG_VALUE_AND_TYPE_OF(d);
                }
            }
        }
    } 



    DEBUG_VALUE_AND_TYPE_OF(d);

    //cs231n();
	/**
    typedef std::tuple<float, float> row_type;
    std::vector<row_type> data;
    std::ifstream is("data/ex1data1.txt");
    if(is.is_open()) {
		for(auto& row : csv::reader<float, float>(is)) {
            data.push_back(row);
		}
    }

    
    DEBUG_VALUE_AND_TYPE_OF(data);

    std::valarray<float> x(data.size() * 2);


    std::for_each(data.begin(), data.end(), [&] (const row_type& row) {
            static int i=0;
            x[i++] = std::get<0>(row);
            x[i++] = std::get<1>(row);
    });

    print_asmatrix(x, 2);
	*/
    return 0;
	

}
