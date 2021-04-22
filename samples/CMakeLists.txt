find_package(X11)

add_subdirectory(data)
add_subdirectory(include)

if(X11_FOUND)
    add_subdirectory(glxgears)
endif(X11_FOUND)