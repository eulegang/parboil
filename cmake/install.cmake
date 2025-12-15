configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parboil.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/parboil.pc
    @ONLY
)

install(TARGETS parboil LIBRARY DESTINATION lib)
install(FILES src/parboil.h DESTINATION include)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/parboil.pc" DESTINATION lib/pkgconfig)


