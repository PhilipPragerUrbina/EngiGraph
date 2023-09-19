project(EngiGraphTests)

add_subdirectory(${PROJECT_SOURCE_DIR}/../external/GoogleTest ./gtest_build)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(EngiGraphTests ${sources})
target_link_libraries(EngiGraphTests gtest gtest_main EngiGraphLib)