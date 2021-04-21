set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)

doxygen_add_docs(doc ${PROJECT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
    DESTINATION "share/doc/${PROJECT_NAME}")
