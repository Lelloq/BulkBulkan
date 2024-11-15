# CMakeList.txt : CMake project for BulkBulkan, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("BulkBulkan")
include_directories (${PROJECT_SOURCE_DIR} Libs)

file(GLOB_RECURSE _source_list ${PROJECT_NAME}/*.cpp ${PROJECT_NAME}/*.h ${PROJECT_NAME}/*.hpp  )
foreach(_source IN ITEMS ${_source_list})
	get_filename_component(_source_path "${_source}" PATH)
	string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
	string(REPLACE "/" "\\" _group_path "${_group_path}")
	source_group("${_group_path}" FILES "${_source}")
endforeach()

find_package(Vulkan REQUIRED)

add_subdirectory(Libs/glfw-3.4)
add_subdirectory(Libs/glm)

add_executable (${PROJECT_NAME} ${_source_list})

target_link_libraries(${PROJECT_NAME} glfw Vulkan::Vulkan)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
