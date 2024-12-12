#!/bin/sh

set -e

clang++ -o gmap-gui main.cpp imgui_impl_glfw.cpp imgui_impl_opengl3.cpp \
    imgui.cpp imgui_draw.cpp imgui_widgets.cpp \
    -I. -lglfw -lGL -ldl -pthread