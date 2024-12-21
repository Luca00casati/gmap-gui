cmake_minimum_required(VERSION 3.17)
project(gmap-gui)

set(CMAKE_CXX_STANDARD 20)
set(LIBRARIES_DIR lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG")

include_directories(src/include)

set(SRC_FILES
    src/main.cpp
    src/genpatch.cpp
    src/strings.cpp
)

set(VENDOR_FILES
    vendor/imgui_impl_glfw.h
    vendor/imgui_impl_glfw.cpp
    vendor/imgui_impl_opengl3.h
    vendor/imgui_impl_opengl3.cpp
    vendor/imgui_impl_opengl3_loader.h
)

add_executable(gmap-gui ${SRC_FILES} ${VENDOR_FILES})

# glfw
set(GLFW_DIR "${LIBRARIES_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# imgui
set(IMGUI_DIR "${LIBRARIES_DIR}/imgui")
add_library("imgui"
        "${IMGUI_DIR}/src/imgui.cpp"
        "${IMGUI_DIR}/src/imgui_draw.cpp"
        "${IMGUI_DIR}/src/imgui_demo.cpp"
        "${IMGUI_DIR}/src/imgui_tables.cpp"
        "${IMGUI_DIR}/src/imgui_widgets.cpp")
target_include_directories("imgui" PRIVATE "${IMGUI_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${IMGUI_DIR}/include")
target_link_libraries(${PROJECT_NAME} "imgui" "${CMAKE_DL_LIBS}")

# nfd
set(NFD_DIR "${LIBRARIES_DIR}/nfd")
add_library("nfd"
        "${NFD_DIR}/src/nfd_common.c")
target_include_directories("nfd" PRIVATE "${NFD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${NFD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "nfd" "${CMAKE_DL_LIBS}")

if (WIN32)
    target_sources(nfd PRIVATE "${NFD_DIR}/src/nfd_win.cpp")
elseif (APPLE)
    target_sources(nfd PRIVATE "${NFD_DIR}/src/nfd_cocoa.m")
elseif (UNIX)
    target_sources(nfd PRIVATE "${NFD_DIR}/src/nfd_zenity.c")
endif()

# Windows-specific options
if(WIN32)
    # disable console
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
endif()