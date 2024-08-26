#pragma once

namespace numeric {
    template <typename T, typename I>
    T mean(I first, I last) {
        return std::accumulate(first, last, static_cast<T>(0)) / std::distance(first, last);
    }

    template <typename T, typename C>
    T mean(const C& in) {
        return mean(std::begin(in), std::end(in));
    }

    template <typename T, typename I>
    T stddev(I first, I last, const T& mean) {
        T std = static_cast<T>(0);
        for(; first != last; ++first) {
            std += (*first - mean) * (*first - mean);
        }
        return std::sqrt(std / std::distance(first - last));
    }

    template <typename T, typename C>
    T stddev(const C& in, const T& mean) {
        return stddev(std::begin(in), std::end(in), mean);
    }

}
