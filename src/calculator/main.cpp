#include "calculator/basic_calculator.hpp"
#include <iostream>

int main() {
    Calc::BasicCalculator calculator; // no need to create instance 
    std::string operations = "1. Add\n2. Subtract\n3. Multiply\n4. Divide\n5. Exit\n";
    std::string askOperation = "Enter choice: ";
    
    std::cout << operations;
    int choice = 0;

    while (true) {
        while (choice > 5 || choice < 1) {
            std::cout << "Enter choice: ";
            std::cin >> choice;
        }

        if (choice == 5) return 0;

        double num1, num2;
        std::cout << "Enter number 1: ";
        std::cin >> num1;
        std::cout << "Enter number 2: ";
        std::cin >> num2;

        double result = 0;
        switch (choice) {
            case 1 : result = calculator.add(num1, num2); break;
            case 2 : result = calculator.substract(num1, num2); break;
            case 3 : result = calculator.multiply(num1, num2); break;
            case 4 : result = calculator.divide(num1, num2); break;
        }

        std::cout << "Result: " << result << "\n\n";
        choice = 0;
    }

    return 0;
}