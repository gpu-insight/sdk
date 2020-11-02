find_path(SISDK_INCLUDE_DIR SDK/context.hpp
          HINTS /usr/include /usr/local/include)
find_library(SISDK_LIBRARIES NAMES sisdk libsisdk
             HINTS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SISDK DEFAULT_MSG
        SISDK_INCLUDE_DIR SISDK_LIBRARIES)

mark_as_advanced(SISDK_INCLUDE_DIR SISDK_LIBRARIES)

