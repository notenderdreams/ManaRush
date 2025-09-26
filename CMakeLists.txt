cmake_minimum_required(VERSION 3.10)
project(ManaRush)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_SOURCE_DIR})

set(CMAKE_C_STANDARD 11)

include_directories(/opt/homebrew/include)

set(SOURCE_FILES
    main.c
    game.c
    character.c
    boss.c
    projectile.c
    map_renderer.c
)

add_executable(ManaRush ${SOURCE_FILES})


if(APPLE)
target_link_libraries(ManaRush
        "-framework OpenGL"
        "-framework Cocoa" 
        "-framework IOKit"
        "-framework CoreVideo"
        /opt/homebrew/lib/libraylib.dylib
        /opt/homebrew/lib/libtmx.dylib
        m dl pthread
    )
endif()


if(WIN32)
    include_directories("C:/raylib/include")
    include_directories("D:/local/tmx/include")
    
    target_link_libraries(${PROJECT_NAME} "C:/raylib/lib/raylib.lib")
    target_link_libraries(${PROJECT_NAME} "D:/local/tmx/lib/tmx.lib")
    target_link_libraries(${PROJECT_NAME} opengl32 gdi32 winmm)
endif()
