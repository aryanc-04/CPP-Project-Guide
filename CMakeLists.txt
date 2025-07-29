cmake_minimum_required(VERSION 3.16)
project(ScientificCalculator 
    VERSION 1.0.0 
    DESCRIPTION "A scientific calculator with expression parsing"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type specific settings
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wconversion -Wsign-conversion
        -Wunused -Wshadow
        $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
        $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native>
    )
endif()

# Find required packages
find_package(Threads REQUIRED)

# Create math utilities library
add_library(math_utils
    src/utils/math_utils.cpp
    src/utils/string_utils.cpp
)

target_include_directories(math_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(math_utils PUBLIC cxx_std_17)

# Create calculator library
add_library(calculator_lib
    src/calculator/basic_calculator.cpp
    src/calculator/scientific_calculator.cpp
    src/calculator/expression_parser.cpp
)

target_include_directories(calculator_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(calculator_lib PUBLIC math_utils)
target_compile_features(calculator_lib PUBLIC cxx_std_17)

# Create main executable
add_executable(calculator src/main.cpp)
target_link_libraries(calculator PRIVATE calculator_lib Threads::Threads)

# Enable testing
enable_testing()
include(CTest)

# Add test executable
if(BUILD_TESTING)
    add_subdirectory(external/catch2)
    
    add_executable(calculator_tests
        tests/test_basic_calculator.cpp
        tests/test_scientific_calculator.cpp
        tests/test_math_utils.cpp
    )
    
    target_link_libraries(calculator_tests PRIVATE 
        calculator_lib
        Catch2::Catch2WithMain
    )
    
    # Discover tests for CTest
    include(Catch)
    catch_discover_tests(calculator_tests)
endif()

# Installation rules
install(TARGETS calculator calculator_lib math_utils
    EXPORT ScientificCalculatorTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

# Export configuration
install(EXPORT ScientificCalculatorTargets
    FILE ScientificCalculatorTargets.cmake
    NAMESPACE ScientificCalculator::
    DESTINATION lib/cmake/ScientificCalculator
)


# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)