find_package(PkgConfig REQUIRED)
pkg_check_modules(GTEST REQUIRED gtest)
if(NOT GTEST_FOUND)
  message(FATAL_ERROR "failed to find gtest on system")
endif()

enable_testing()

add_executable(tests)
target_link_libraries(tests PRIVATE parboil)
file(GLOB TESTS_SRC "tests/*.cpp")
target_sources(tests PRIVATE ${TESTS_SRC})

target_link_libraries(tests PRIVATE ${GTEST_LDFLAGS})
target_compile_options(tests PRIVATE ${GTEST_CFLAGS})

add_test(tests tests)
