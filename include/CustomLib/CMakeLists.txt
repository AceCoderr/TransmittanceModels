set(
LIBRARY_SOURCES
"Mylib.cpp"
)
set(
LIBRARY_HEADERS
"Mylib.h"
"shader_m.h"
"shader.h"
"camera.h"
"model.h"
"mesh.h"
"filesystem.h"
"imgui_impl_glfw.h"
"GUI.h"
)
set(
LIBRARY_INCLUDES
"./"
)
# STATIC/DYNAMIC LINKING OF LIBRARIES
add_library(
${LIBRARY_NAME} STATIC
${LIBRARY_SOURCES}
${LIBRARY_HEADERS}
)
set(
    EXTERN_LIBS
    glm::glm
    assimp
    glfw
)
# Including the current directory for libraries
target_include_directories(
${LIBRARY_NAME} PUBLIC
${LIBRARY_INCLUDES}
)
# Linking the external libraries that we got from FetchContent
# Linking the libraries to our custom library (LIBRARY_NAME)
# We use project_name::library_name (Both can be found in the CMakeLists.txt for the library)
target_link_libraries(
    ${LIBRARY_NAME} PUBLIC
    ${EXTERN_LIBS}
    ${EXTERN_LIBRARY_NAME}

)
