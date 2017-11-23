//#include <tiny_dnn/tiny_dnn.h>
//
//
//attempt at copying: http://neuralnetworksanddeeplearning.com/chap1.html
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <cmath>
#define _DEBUG
#include "log.h"

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
#include "range.h"
#include "zip.h"
#include "split.h"

//#include "tensor.hpp"
#define argmax(x)   std::max_element(((x)).begin(), ((x)).end())

template <typename T>
struct basic_ndarray {
    typedef std::valarray<std::size_t> index_type;
    typedef std::valarray<T> data_type;

	index_type _shape;
	data_type _data;

//public:

	std::slice_array<T> row(std::size_t ix) {
	    return _data[std::slice(ix * shape[1], shape[1], 1)];
	}

	std::slice_array<T> column(unsigned ix) {
	    unsigned iy = shape[0];
    	return _data[std::slice(ix, iy, shape[1])];
	}

	static
	std::valarray<T> transposed(std::valarray<T>& arr, unsigned dim) {
	    std::valarray<T> res(arr.size());
    	unsigned rows = arr.size() / dim;
	    for(unsigned i=0; i < rows; i++) {
    	    column(res, i, rows) = row(arr, i, dim);
	    }
    	return res;
	}
    
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


	void identity(std::valarray<T>& a, unsigned dim) {
    	std::fill(std::begin(a), std::end(a), 0.0f);
	    a[std::slice(0, a.size(), dim + 1)] = 1.0f; 
	}

	T trace(const std::valarray<T>& a, unsigned dim) {
    	return a[std::gslice(0, {dim}, {dim+1})].sum();
	}


	std::size_t indexnd(const std::valarray<std::size_t>& ix) {
    	assert(_shape.size() == ix.size());
	    // nd + Nd * (nd-1 + Nd-1 * (nd-2 + Nd-2 * (nd-3 ... ))) 
#if 0 
    	std::size_t ii = 0;
	    std::size_t ml = 1;
     
    	for(auto i: range(_shape.size())) {
        	assert(ix[i] < _shape[i]);
	        ii += ix[i] * ml; 
    	    ml *= dim[i];
    	}
	    return ii;
#else
	    std::vector<std::size_t> muls = { 1 };
    	std::partial_sum(std::begin(_shape), std::end(_shape) - 1, std::back_inserter(muls), 
        	    [&] (const std::size_t& a, const std::size_t& b) {
            	    return a * b;
	            });
    	return std::inner_product(begin(ix), std::end(ix), std::begin(muls), 0);
#endif
	}

    inline const index_type shape() const {
        return shape;
    }

};



template <typename T>
std::ostream& operator << (std::ostream& os, const std::valarray<T>& a) {
    std::copy(std::begin(a), std::prev(std::end(a)), std::ostream_iterator<float>(os, ","));
    os << *std::prev(std::end(a));
    return os;
}

template <typename T>
std::ostream& operator << (std::ostream& os, const basic_ndarray<T>& va) {
    return os;
}

#include <fstream>
#include "csv.hpp"


int main(int argc, char* argv[]) {
    DEBUG_METHOD();
    typedef std::tuple<float, float> row_type;
    std::vector<row_type> data;
    std::ifstream is("data/ex1data1.txt");
    if(is.is_open()) {
		for(auto& row : csv::reader<float, float>(is)) {
            data.push_back(row);
		}
    }



    
#if 1
    //DEBUG_VALUE_AND_TYPE_OF(data);

#else
    std::valarray<float> X(data.size() * 2);
    basic_ndarray<float> x = { { data.size(), 2 }, X };
    std::for_each(data.begin(), data.end(), [&] (const row_type& row) {
            static int i=0;
            x._data[i++] = std::get<0>(row);
            x._data[i++] = std::get<1>(row);
    });
    //print_asmatrix(x, 2);
    DEBUG_VALUE_OF(x);
#endif
    return 0;
	

}
