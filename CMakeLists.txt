cmake_minimum_required(VERSION 3.10)
project(parboil 
  LANGUAGES CXX
  VERSION 0.1.0
  DESCRIPTION "Parser combinators"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/lib.cmake)
include(cmake/test.cmake)
