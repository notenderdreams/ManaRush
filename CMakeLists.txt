cmake_minimum_required(VERSION 3.10)
project(game C)
set(CMAKE_C_STANDARD 11)
set(SOURCES
    main.c
    music.c
    interface.c
    character.c
)
add_executable(${PROJECT_NAME} ${SOURCES})
if(APPLE)
    include_directories(/opt/homebrew/opt/raylib/include)
    target_link_libraries(${PROJECT_NAME} /opt/homebrew/opt/raylib/lib/libraylib.dylib)
    target_link_libraries(${PROJECT_NAME}
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()
if(WIN32)
    include_directories("C:/raylib/include")
    target_link_libraries(${PROJECT_NAME} "C:/raylib/lib/raylib.lib")
    target_link_libraries(${PROJECT_NAME} opengl32 gdi32 winmm)
endif()
