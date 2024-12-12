# The name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# The target architecture
set(CMAKE_SYSTEM_PROCESSOR x86_64)  # 64-bit Windows target

set(CMAKE_EXE_LINKER_FLAGS "-static")  # For static linking

# MinGW prefix (adjust according to your system)
set(MINGW_PREFIX "/usr")

# Set MinGW compilers for 64-bit
set(CMAKE_C_COMPILER   ${MINGW_PREFIX}/bin/x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER ${MINGW_PREFIX}/bin/x86_64-w64-mingw32-g++)

# Set the preprocessor for 64-bit
set(CMAKE_C_PREPROCESSOR ${MINGW_PREFIX}/bin/x86_64-w64-mingw32-cpp)
set(CMAKE_CXX_PREPROCESSOR ${MINGW_PREFIX}/bin/x86_64-w64-mingw32-cpp)

# Where is the target environment located?
set(CMAKE_FIND_ROOT_PATH  ${MINGW_PREFIX})

# Adjust the default behavior of the FIND_XXX() commands:
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)  # Do not search for programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)  # Search libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)  # Search headers in the target environment
