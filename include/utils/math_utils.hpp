#pragma once
#include <cmath>

namespace Calc::Utils {

// Floating point comparison
bool isZero(double value, double epsilon = 1e-9);
bool areEqual(double a, double b, double epsilon = 1e-9);

// Mathematical Functions
double factorial(int n);
double power(double base, int exponent);
double degreeToRadian(double degrees);
double radianToDegree(double radians);

// Validation
bool isFinite(double value);
bool isValidForLog(double value);
bool isValidForSqrt(double value);
}