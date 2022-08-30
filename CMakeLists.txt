cmake_minimum_required (VERSION 3.8)

project ("Version1")

# Add source to this project's executable.                                                                                           
find_library (glfw3library NAMES glfw3 PATHS ${PROJECT_SOURCE_DIR})
#find_library (lualib NAMES lua54 PATHS ${PROJECT_SOURCE_DIR})
add_executable (Version1 "main.cpp" "glad.c")
# target_link_libraries(Version1 ${lualib})  
target_link_libraries(Version1 ${glfw3library})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Version1 PROPERTY CXX_STANDARD 20)
endif()