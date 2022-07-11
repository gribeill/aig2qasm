cmake_minimum_required(VERSION 3.10)

project(aig2qasm VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(CATERPILLAR_TEST OFF CACHE BOOL "")
add_subdirectory(lib/caterpillar)

add_subdirectory(lib/cxxopts)

add_executable(aig2qasm aig2qasm.cxx)

# Guilhem is bad at CMake...

target_link_libraries(aig2qasm PUBLIC caterpillar)
target_include_directories(aig2qasm PUBLIC
							"${PROJECT_BINARY_DIR}"
							"${PROJECT_SOURCE_DIR}/lib/caterpillar/include/caterpillar"
							)


target_link_libraries(aig2qasm PUBLIC cxxopts)
target_include_directories(aig2qasm PUBLIC
							"${PROJECT_BINARY_DIR}"
							"${PROJECT_SOURCE_DIR}/lib/cxxopts/include"
							)

target_link_libraries(aig2qasm PUBLIC mockturtle)
