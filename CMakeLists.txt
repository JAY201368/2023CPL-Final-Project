cmake_minimum_required(VERSION 3.26)
project(Final_Project C)

set(CMAKE_C_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(
        ${SDL2_INCLUDE_DIRS}
        /opt/homebrew/Cellar/sdl2_image/2.8.2/include
        /opt/homebrew/Cellar/sdl2_ttf/2.22.0/include
)

add_executable(game main.c)
target_link_libraries(game PRIVATE
        ${SDL2_LIBRARIES}
        /opt/homebrew/Cellar/sdl2_image/2.8.2/lib/libSDL2_image-2.0.0.dylib
        /opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib/libSDL2_ttf-2.0.0.dylib
)