cmake_minimum_required(VERSION 3.20)
project(win_learn)

set(CMAKE_CXX_STANDARD 20)

add_library(Base INTERFACE)
target_sources(Base INTERFACE
	Main.cpp
	BaseApp.cpp
	BaseApp.h
)

add_subdirectory(utils)
add_subdirectory(3rdParty)

target_include_directories(Base INTERFACE / 3rdParty/FreeType/include utils)
target_link_libraries(Base INTERFACE D3D11.lib D3DCompiler.lib DirectXTK freetype)
target_compile_definitions(Base INTERFACE UNICODE)
target_compile_options(Base INTERFACE /W4)

macro(AddExe name sources)
	add_executable(${name} WIN32 ${sources})
	target_link_libraries(${name} PRIVATE Base)
	target_compile_definitions(${name} PRIVATE APP_NAME=L"${name}")
	
	add_custom_command(TARGET ${name} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/Assets/ $<TARGET_FILE_DIR:${name}>/Assets)
	
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Assets)
		add_custom_command(TARGET ${name} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_CURRENT_SOURCE_DIR}/Assets/ $<TARGET_FILE_DIR:${name}>/Assets)
	endif()
endmacro()

add_subdirectory(demos)