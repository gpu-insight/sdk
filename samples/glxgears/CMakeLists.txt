add_executable(glxgears
    glxgears.cpp
    save2bmp.cpp)

target_link_libraries(glxgears
    PRIVATE sis
    PRIVATE ${X11_LIBRARIES})
target_include_directories(glxgears
    PRIVATE ../include
    PRIVATE ${X11_INCLUDE_DIR})

install(TARGETS glxgears
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
