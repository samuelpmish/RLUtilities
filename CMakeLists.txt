cmake_minimum_required(VERSION 3.8)
project(RLUtilities LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(TARGET_LANGUAGE "cpp" CACHE STRING "Build shared library for \"cpp\", \"python\" or \"both\"")
set_property(CACHE TARGET_LANGUAGE PROPERTY STRINGS "cpp" "python" "both")
message(STATUS "Building libraries for ${TARGET_LANGUAGE}")

add_compile_definitions(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE extern/asio/include/)

add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE extern/rapidjson/include/)

set(CMAKE_CXX_STANDARD 17)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

file(GLOB_RECURSE rlu_sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/inc/*.h)

if (TARGET_LANGUAGE STREQUAL "cpp" OR TARGET_LANGUAGE STREQUAL "both")

  add_library(rlutilities_cpp ${rlu_sources})
  target_include_directories(rlutilities_cpp PUBLIC inc)
  target_compile_features(rlutilities_cpp PUBLIC cxx_std_17)
  target_link_libraries(rlutilities_cpp PUBLIC asio rapidjson)
  
  if (WITH_TESTS)
  
    enable_testing()
  
    file(GLOB cc_tests "${PROJECT_SOURCE_DIR}/tests/*.cc")
    
    foreach(filename ${cc_tests})		
    		
      # remove the filetype suffix		
      string(REPLACE ".cc" "" testname ${filename})		
      		
      # remove the prefix		
      string(REPLACE ${PROJECT_SOURCE_DIR}/tests/ "" testname ${testname})		
      		
      # use the remaining string to name the binary file		
      add_executable(${testname} ${filename})		
      add_test(${testname} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${testname})		
    		
      target_include_directories(${testname} PUBLIC inc/)
      target_link_libraries(${testname} rlutilities_cpp)
     		
    endforeach(filename ${cc_tests})
  
  endif()

endif() 


if (TARGET_LANGUAGE STREQUAL "python" OR TARGET_LANGUAGE STREQUAL "both")

  add_subdirectory(python/pybind11)

  add_compile_options($<$<CXX_COMPILER_ID:GNU>:-D_hypot=hypot>)

  file(GLOB_RECURSE rlu_bindings ${PROJECT_SOURCE_DIR}/python/src/*.cc)

  pybind11_add_module(rlutilities ${rlu_bindings} ${rlu_sources})
  target_include_directories(rlutilities PUBLIC inc)
  target_compile_definitions(rlutilities PUBLIC -DGENERATE_PYTHON_BINDINGS)
  target_compile_definitions(rlutilities PUBLIC -DHAVE_SNPRINTF=1)
  target_link_libraries(rlutilities PUBLIC asio rapidjson)

  set_target_properties(rlutilities PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/python/rlutilities)
  set_target_properties(rlutilities PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/python/rlutilities)

  file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${PROJECT_SOURCE_DIR}/python/rlutilities)

endif()

set(ASSET_DIR ${PROJECT_SOURCE_DIR}/assets/)
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/inc/misc/config.h.in"
                "${CMAKE_CURRENT_SOURCE_DIR}/inc/misc/config.h")
