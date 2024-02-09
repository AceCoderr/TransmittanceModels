set(
EXEC_SOURCES
"main.cpp"
"imgui_impl_glfw.cpp"
"GUI.cpp"
)
# Adding the main file as an executable to our project
add_executable(
${EXECUTABLE_NAME}
${EXEC_SOURCES}
)
# We have inheritance modes - PUBLIC, PRIVATE, INTERFACE
target_link_libraries(
${EXECUTABLE_NAME} PUBLIC
${LIBRARY_NAME}
)
