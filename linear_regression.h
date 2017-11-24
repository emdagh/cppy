#pragma once

// https://github.com/aluxian/CPP-ML-LinearRegression/blob/master/LinearRegression.cpp
// https://helloacm.com/cc-linear-regression-tutorial-using-gradient-descent/
// http://www.johnwittenauer.net/machine-learning-exercises-in-python-part-1/

namespace numeric {

    template <typename T>
    class linear_regression {
        typedef T float_type;
        typedef std::valarray<float_type> va_type;
    public:
        linear_regression(const va_type& x, const va_type& y);


        void predict(float_type x[], float_type theta[], int m, float_type* res) {
            
        }

        void gradient_descent(float_type x[], const va_type& y, int iters, int m) {
            float_type theta[2] = { 1, 1 };

            std::valarray<T> predictions(m);

            for(int i=0; i < iters; ++i) {
                // calculate predictions
                predict(x, theta, m, &predictions[0]);
                auto diff = predictions - y;                
            }
        }
    };
}
