CPMAddPackage(
  NAME icu
  GITHUB_REPOSITORY AiMiDi/icu-cmake
  GIT_TAG aec5295e98979f2bcf3646c834e8e1e31c70b485
  OPTIONS
    "ICU_BUILD_I18N OFF"
    "ICU_BUILD_IO OFF")