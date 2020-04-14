project(game VERSION 0.1.0 LANGUAGES CXX)

# LIST HEADER AND SOURCE FILES ################################################

file(GLOB HEADER_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

file(GLOB COFIGURED_HEADERS CONFIGURE_DEPENDS 
	"${CMAKE_BINARY_DIR}/config/*.h")

#include_directories("${PROJECT_BINARY_DIR}/config/")
add_executable(game ${SOURCE_LIST} ${HEADER_LIST} ${COFIGURED_HEADERS})

get_property(OPENAL_INCLUDE_DIR GLOBAL PROPERTY OPENAL_INCLUDE_DIR)
get_property(GLM_INCLUDE_DIR GLOBAL PROPERTY GLM_INCLUDE_DIR)

# TARGET SETTINGS #############################################################⌈

# include the directory with the new files
target_include_directories(game
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/include/"
		"${CMAKE_BINARY_DIR}/config/"
		"${GLM_INCLUDE_DIR}"
)

target_link_libraries(game 
	PUBLIC ecs renderer lua audio glm OpenAL assetmanager steam_controller)

target_compile_features(game PRIVATE cxx_std_17)

set_target_properties(game PROPERTIES OUTPUT_NAME "wardrobe")
set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

# MSVC HIDE COMMAND LINE ######################################################

option(HIDE_CRT_WIN32_MSVC "Hide CRT when building with MSVC" OFF)

if(HIDE_CRT_WIN32_MSVC)
	if(MSVC)
		set(CMAKE_EXE_LINKER_FLAGS
			"${CMAKE_EXE_LINKER_FLAGS}
			/SUBSYSTEM:WINDOWS
			/ENTRY:mainCRTStartup"
		)
	endif()
endif()

# COPY THE GAME ASSETS ########################################################

file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#add_custom_command(TARGET game POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_if_different
#		"${CMAKE_SOURCE_DIR}/Debug"
#		$<TARGET_FILE_DIR:game>
#)

add_custom_command(TARGET game POST_BUILD 
               COMMAND ${CMAKE_COMMAND} -E copy_if_different
               $<TARGET_FILE:OpenAL>              
               $<TARGET_FILE_DIR:game>)