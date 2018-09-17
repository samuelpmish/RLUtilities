cmake_minimum_required(VERSION 3.8)
project(Lobot)

add_definitions(-DASSET_PATH="${PROJECT_SOURCE_DIR}/assets/")

add_subdirectory(tests)
add_subdirectory(pybind11)

add_library(bot_utils STATIC
  ${PROJECT_SOURCE_DIR}/src/car.cpp
  ${PROJECT_SOURCE_DIR}/src/ball.cpp
  ${PROJECT_SOURCE_DIR}/src/bvh.cpp
  ${PROJECT_SOURCE_DIR}/src/pitch.cpp
  ${PROJECT_SOURCE_DIR}/src/mesh.cpp
  ${PROJECT_SOURCE_DIR}/src/primitives.cpp
)

target_include_directories(bot_utils PUBLIC inc/)
target_compile_features(bot_utils PUBLIC cxx_std_11)

pybind11_add_module(${PROJECT_NAME} src/lobot.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE bot_utils)
target_include_directories(${PROJECT_NAME} PRIVATE inc/)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

# cmake . -G"Visual Studio 15 2017 Win64"
# cmake --build . --config Release
