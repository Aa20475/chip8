# CMakeList.txt : CMake project for chip8, include source and define
# project specific logic here.
#

set(SDL2_DIR "./include/SDL2/")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
message(SDL2_INCLUDE_DIRS="${SDL2_INCLUDE_DIRS}")
message(SDL2_LIBRARIES="${SDL2_LIBRARIES}")
message(SDL2_DLL="${SDL2_DLL}")

set(SOURCES "chip8.cpp" "chip8.h" "system.h" "system.cpp" "display.h" "display.cpp")
# Add source to this project's executable.
add_executable (chip8 ${SOURCES})
target_include_directories(chip8 PRIVATE .)
target_link_libraries(chip8 ${SDL2_LIBRARIES})

add_custom_command(TARGET chip8 POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        ${SDL2_DLL}      # <--this is in-file
        $<TARGET_FILE_DIR:chip8>)                 # <--this is out-file path

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET chip8 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
