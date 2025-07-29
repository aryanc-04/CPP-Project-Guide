#include "utils/math_utils.hpp"
#include <stdexcept>
#include <cmath>

namespace Calc::Utils {

bool isZero(double value, double epsilon) {
    return std::abs(value) < epsilon;
}

bool areEqual(double a, double b, double epsilon) {
    return std::abs(a - b) < epsilon;
}

double factorial(int n) {
    if (n < 0) {
        throw std::invalid_argument("Factorial undefined for negative numbers");
    }

    if (n > 170) { // Prevent overflow for double
        throw std::overflow_error("Factorial too large for double precision")  ;
    }

    if (n <= 1) return 1.0;

    double result = 1.0;
    for (int i = 2; i <= n; ++i) 
        result *= i;

    return result;
}

double power(double base, int exponent) {
    if (exponent == 0) return 1.0;

    if (exponent < 0) {
        if (isZero(base)) {
            throw std::invalid_argument("Cannot raise zero to negative power");
        }

        return 1.0 / power(base, -exponent);
    }

    double result = 1.0;
    double current_power = base;

    // binary exponentiation
    while (exponent > 0) {
        if (exponent & 1) 
            result *= current_power;
        
        current_power *= current_power;
        exponent >>= 1;
    }

    return result;
}

double degreeToRadian(double degrees) {
    return degrees * M_PI / 180.0;
}

double radianToDegree(double radians) {
    return radians * 180.0 / M_PI;
}

bool isFinite(double value) {
    return std::isfinite(value);
}

bool isValidForLog(double value) {
    return value > 0.0 && std::isfinite(value);
}

bool isValidForSqrt(double value) {
    return value >= 0.0 && std::isfinite(value);
}

}