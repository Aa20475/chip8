# CMakeList.txt : CMake project for chip8, include source and define
# project specific logic here.
#

set(SOURCES "chip8.cpp" "chip8.h" "system.h" "system.cpp")
# Add source to this project's executable.
add_executable (chip8 ${SOURCES})
target_include_directories(chip8 PRIVATE .)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET chip8 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
