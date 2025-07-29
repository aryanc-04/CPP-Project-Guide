#pragma once

#include <stdexcept>

namespace Calc {


class BasicCalculator {

public:
    // Basic arithmetic operations
    double add(double a, double b) const;
    double substract(double a, double b) const;
    double multiply(double a, double b) const;
    double divide(double a, double b) const;
    
    // Memory operations
    void memoryStore(double value);
    double memoryRecall() const;
    void memoryClear();
    
    // Utility
    void clear();
    double getLastResult() const;

private:
    double memory_ = 0.0;
    double last_result_ = 0.0;

    void updateLastResult(double result);
};

// Custom exception for calculation errors
class CalculatorError : public std::runtime_error {

public:
    explicit CalculatorError (const std::string& message) 
        : std::runtime_error(message) {}
};

}