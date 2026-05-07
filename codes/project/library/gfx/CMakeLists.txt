add_library(gfx
    gfx.c
)

target_include_directories(gfx
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(gfx
    PUBLIC
        ili9341
)

