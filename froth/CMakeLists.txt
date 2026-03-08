cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(HOME "$ENV{HOME}")

project(Froth VERSION 0.0.1)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(FROTH_PLATFORM_MACOS TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(FROTH_PLATFORM_LINUX TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(FROTH_PLATFORM_WIN TRUE)
else()
  message("WARNING: Target platform '${CMAKE_SYSTEM_NAME}' not supported")
endif()

configure_file(Defines.h.in Defines.h)

file(GLOB_RECURSE ENGINE_SOURCES src/**.cpp)
file(GLOB_RECURSE ENGINE_HEADERS src/**.h)

add_library(${PROJECT_NAME}
  ${ENGINE_SOURCES} ${ENGINE_HEADERS}
  dependencies/vendor/SPIRV-Reflect/spirv_reflect.cpp
  dependencies/vendor/SPIRV-Reflect/spirv_reflect.h
  dependencies/vendor/stb/stb/stb_image.h
  dependencies/vendor/tiny_obj_loader/tiny_obj_loader.h
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Debug>:${PROJECT_NAME}_BUILD_DEBUG=1>
)


target_include_directories(${PROJECT_NAME} PUBLIC 
  ${PROJECT_BINARY_DIR}
  ./ 
  dependencies/vendor/glm
  dependencies/vendor/glfw/include
  dependencies/vendor/shaderc/libshaderc/include
  dependencies/vendor/SPIRV-Reflect
  dependencies/vendor/stb
  dependencies/vendor/tiny_obj_loader
)

add_subdirectory(dependencies/vendor/glm)
add_subdirectory(dependencies/vendor/glfw)

set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
add_subdirectory(dependencies/vendor/shaderc)

target_link_libraries(${PROJECT_NAME} PUBLIC glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE shaderc)

find_package(Vulkan REQUIRED)

if(Vulkan_FOUND)
  target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
else()
  message(STATUS "Vulkan not found")
endif()

