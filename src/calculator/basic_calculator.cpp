#include "calculator/basic_calculator.hpp"
#include "utils/math_utils.hpp"
#include <cmath>

namespace Calc {

double BasicCalculator::add(double a, double b) const {
    double result = a + b;

    // check for overflow
    if (std::isinf(result)) {
        throw CalculatorError("Addition Overflow");
    }

    const_cast<BasicCalculator*>(this)->updateLastResult(result);
    return result;
}

double BasicCalculator::substract(double a, double b) const { 
    double result = a - b;

    if (std::isinf(result)) {
        throw CalculatorError("Substraction Overflow");
    }

    const_cast<BasicCalculator*>(this)->updateLastResult(result);
    return result;
}

double BasicCalculator::multiply(double a, double b) const { 
    double result = a * b;

    if (std::isinf(result)) {
        throw CalculatorError("Multiplication Overflow");
    }

    const_cast<BasicCalculator*>(this)->updateLastResult(result);
    return result;
}

double BasicCalculator::divide(double a, double b) const { 
    if (Utils::isZero(b)) {
        throw CalculatorError("Division by zero");
    }
    
    double result = a / b;
    const_cast<BasicCalculator*>(this)->updateLastResult(result);
    return result;
}

void BasicCalculator::memoryStore(double value) {
    memory_ = value;
}

double BasicCalculator::memoryRecall() const {
    return memory_;
}

void BasicCalculator::memoryClear() {
    memory_ = 0.0;
}

void BasicCalculator::clear() {
    last_result_ = 0.0;
}

double BasicCalculator::getLastResult() const {
    return last_result_;
}

void BasicCalculator::updateLastResult(double result) {
    last_result_ = result;
}

}

