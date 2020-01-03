#pragma once

#if defined(__cpp_lib_filesystem)
#  define MCFILE_INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#elif defined(__cpp_lib_experimental_filesystem)
#  define MCFILE_INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
#elif !defined(__has_include)
#  define MCFILE_INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
#elif __has_include(<filesystem>)
#  ifdef _MSC_VER
#    if __has_include(<yvals_core.h>) // for _HAS_CXX17
#      include <yvals_core.h>
#      if defined(_HAS_CXX17) && _HAS_CXX17
#        define MCFILE_INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#      endif
#    endif
#    ifndef MCFILE_INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
#      define MCFILE_INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
#    endif
#  else
#    define MCFILE_INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#  endif
#elif __has_include(<experimental/filesystem>)
#  define MCFILE_INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
#else
#  error Fatal: filesystem header not found
#endif

#if MCFILE_INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
#  include <experimental/filesystem>
namespace mcfile {
namespace detail {
    namespace filesystem = std::experimental::filesystem;
} // namespace detail
} // namespace mcfile
#else
#  include <filesystem>
namespace mcfile {
namespace detail {
    namespace filesystem = std::filesystem;
} // namespace detail
} // namespace mcfile
#endif
