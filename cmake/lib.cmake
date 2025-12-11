
file(GLOB PARBOIL_SRC "src/*.cpp")
add_library(parboil SHARED ${PARBOIL_SRC})

include_directories(src/)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
