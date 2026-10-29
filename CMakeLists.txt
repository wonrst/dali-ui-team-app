cmake_minimum_required(VERSION 3.20)
project(dali-ui-team-app VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(CTest)

find_package(PkgConfig REQUIRED)
pkg_check_modules(UI REQUIRED IMPORTED_TARGET dali2-ui-foundation dali2-adaptor dali2-core)
pkg_check_modules(TEAM REQUIRED IMPORTED_TARGET team-application>=1.3.6)
pkg_check_modules(APP_CORE REQUIRED IMPORTED_TARGET app-core-cpp)
find_program(MSGFMT msgfmt REQUIRED)

foreach(member main first second)
  add_library(dali-ui-team-${member} SHARED src/${member}-member.cpp src/window-navigation.cpp)
  target_link_libraries(dali-ui-team-${member} PRIVATE PkgConfig::UI PkgConfig::TEAM)
  target_compile_definitions(dali-ui-team-${member} PRIVATE HIDE_DALI_INTERNALS
    LOCALE_DIR="${CMAKE_INSTALL_PREFIX}/res/locale")
  target_compile_options(dali-ui-team-${member} PRIVATE -Wall -Wextra -Wpedantic -fvisibility=hidden)
  target_link_options(dali-ui-team-${member} PRIVATE -Wl,-z,defs)
  if(member STREQUAL "main")
    target_link_libraries(dali-ui-team-${member} PRIVATE PkgConfig::APP_CORE)
  endif()
  install(TARGETS dali-ui-team-${member} LIBRARY DESTINATION bin)
endforeach()

foreach(locale en_US ko_KR ar_AE)
  set(mo "${CMAKE_CURRENT_BINARY_DIR}/locale/${locale}/LC_MESSAGES/${PROJECT_NAME}.mo")
  add_custom_command(OUTPUT "${mo}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/locale/${locale}/LC_MESSAGES"
    COMMAND ${MSGFMT} --check --check-format -o "${mo}" "${CMAKE_CURRENT_SOURCE_DIR}/po/${locale}.po"
    DEPENDS "po/${locale}.po" VERBATIM)
  list(APPEND catalogs "${mo}")
  install(FILES "${mo}" DESTINATION "res/locale/${locale}/LC_MESSAGES")
endforeach()
add_custom_target(catalogs ALL DEPENDS ${catalogs})

if(BUILD_TESTING)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  add_test(NAME catalogs-and-api COMMAND ${Python3_EXECUTABLE}
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/check-catalogs.py" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
endif()
