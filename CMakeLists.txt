# CMakeLists files in this project can
# refer to the root source directory of the project as ${WARDROBE_SOURCE_DIR}
# and to the root binary directory of the project as ${WARDROBE_BINARY_DIR}.

# 3.15.5 -- fix for MSVC2019
# https://gitlab.kitware.com/cmake/cmake/issues/19829
cmake_minimum_required(VERSION 3.15.5 FATAL_ERROR)
project(WARdrobe 
	VERSION 0.1.0.35
	DESCRIPTION "Beat 'em up"
	LANGUAGES CXX)

# SET THE INSTALL DIRECTORY ###################################################

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX 
		"${PROJECT_BINARY_DIR}/installs" 
		CACHE PATH 
		"<comment>" 
		FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# REQUIRE OUT-OF-SOURCE BUILDS ################################################

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
	message(
		FATAL_ERROR 
		"You cannot build in a source directory (or any directory with \
		a CMakeLists.txt file). Please make a build subdirectory. Feel \
		free to remove CMakeCache.txt and CMakeFiles.")
endif()

# PROJECT VERSION #############################################################

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 2)
set(VERSION_TWEAK 35)

# TIMESTAMP ###################################################################

string(TIMESTAMP TODAY "%y%m%d")
string(TIMESTAMP NOW   "%H%M%S")

# OPTIONS #####################################################################

# project building
option(GIT_SUBMODULE       "Check submodules during build"  ON )
option(BUILD_WARDROBE_DOC  "Build WARdrobe documentation"   OFF)

# components to include
option(INCLUDE_DEBUG_UI    "Include debug interface."       ON )

# option to set the internal encoding of CMake to UTF-8
option(CMAKE_ENCODING_UTF8 "Use UTF-8 encoding internally." ON )

# packaging
if(WIN32)
	option(USE_WIX_TOOLSET "Use the WIX toolset to generate the installer" ON)
endif(WIN32)

# UTILITY FUNCTIONS ###########################################################

function(msg)
	list(GET ARGV 0 MessageType)
	list(REMOVE_AT ARGV 0)
	if(MessageType STREQUAL HEADER)
		execute_process(
			COMMAND 
			${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold "## ${ARGV}")
	elseif(MessageType STREQUAL ATTENTION)
		execute_process(
			COMMAND 
			${CMAKE_COMMAND} -E cmake_echo_color --red --bold ">> ${ARGV}")
	elseif(MessageType STREQUAL WARNING)
		execute_process(
			COMMAND 
			${CMAKE_COMMAND} -E cmake_echo_color --yellow --bold "?? ${ARGV}")
	elseif(MessageType STREQUAL STATUS)
		execute_process(
			COMMAND 
			${CMAKE_COMMAND} -E cmake_echo_color --blue --bold "-- ${ARGV}")
	else()
		message(STATUS "${MessageType}")
	endif()
endfunction()

# FIND THE REQUIRED PACKAGES ##################################################

# OpenGL
find_package(OpenGL REQUIRED)

# AUTOMATICALLY INITIALIZE THE SUBMODULES #####################################

find_package(Git QUIET)

if(EXISTS "${PROJECT_BINARY_DIR}/config/SubmoduleInitStatus.txt")
	file(STRINGS "${PROJECT_BINARY_DIR}/config/SubmoduleInitStatus.txt" 
		"${SUBMODULES_UPDATED}")
	# remove trailing whitespaces
	string(STRIP SUBMODULES_UPDATED SUBMODULES_UPDATED)
else()
	msg(STATUS "Found no information about submodules' initialization...")
	set(SUBMODULES_UPDATED "0")
	file (WRITE "${PROJECT_BINARY_DIR}/config/SubmoduleInitStatus.txt" 
		"${SUBMODULES_UPDATED}")
endif()

if(SUBMODULES_UPDATED STREQUAL "0")
	if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	# Update submodules as needed
		if(GIT_SUBMODULE)
			msg(STATUS "Submodule update")
			execute_process(
				COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				RESULT_VARIABLE GIT_SUBMOD_RESULT)
			execute_process(
				COMMAND ${GIT_EXECUTABLE} submodule status --cached --recursive
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				RESULT_VARIABLE GIT_SUBMOD_RESULT)
			if(NOT GIT_SUBMOD_RESULT EQUAL "0")
				message(FATAL_ERROR 
					"git submodule update --init failed with \
					${GIT_SUBMOD_RESULT}, please checkout submodules")
			endif()
		endif()
	endif()
else()
	msg(STATUS "Git submodules registered as initialized, skip update.")
endif()

# VERBOSE MODE ################################################################

set(CMAKE_VERBOSE_MAKEFILE ON)

# UTF-8 ENCODING ##############################################################

mark_as_advanced(CMAKE_ENCODING_UTF8)
if(CMAKE_ENCODING_UTF8)
	set(KWSYS_ENCODING_DEFAULT_CODEPAGE CP_UTF8)
endif()

# CMAKE-GENERATED C HEADERS ###################################################

file(READ "${PROJECT_SOURCE_DIR}/settings/DefaultSettings.toml" 
		WARDROBE_DEFAULT_SETTINGS_STRING)

string(REGEX REPLACE "\n" "\\\\n\\\\\n" 
	WARDROBE_DEFAULT_SETTINGS_STRING 
	"${WARDROBE_DEFAULT_SETTINGS_STRING}")

string(REGEX REPLACE "\"" "\\\\\"" 
	WARDROBE_DEFAULT_SETTINGS_STRING 
	"${WARDROBE_DEFAULT_SETTINGS_STRING}")

configure_file (
	"${PROJECT_SOURCE_DIR}/config/Version.h.in"
	"${PROJECT_BINARY_DIR}/config/Version.h"
)

configure_file (
	"${PROJECT_SOURCE_DIR}/config/Constants.h.in"
	"${PROJECT_BINARY_DIR}/config/Constants.h"
)

configure_file (
	"${PROJECT_SOURCE_DIR}/config/Enums.h.in"
	"${PROJECT_BINARY_DIR}/config/Enums.h"
)

file(GLOB 
	COFIGURED_HEADERS 
	CONFIGURE_DEPENDS 
	"${PROJECT_BINARY_DIR}/config/*.h")

set_property(GLOBAL PROPERTY 
	COFIGURED_HEADERS 
	"${COFIGURED_HEADERS}")

# BUILD NUMBER GENERATION #####################################################

# get the hash of the current git commit
execute_process(
	COMMAND
		git rev-parse --short HEAD
	RESULT_VARIABLE
		SHORT_HASH_RESULT
	OUTPUT_VARIABLE
		SHORT_HASH)

# strip trailing whitespaces from the git output
string(REGEX REPLACE "\n$" "" SHORT_HASH "${SHORT_HASH}")

# check if building from a new commit
# WARNING: works only if the build was not regenerated
if(EXISTS "${PROJECT_BINARY_DIR}/config/OldGitHash.txt")
	file (STRINGS "${PROJECT_BINARY_DIR}/config/OldGitHash.txt" OLD_HASH)
else()
	set(OLD_HASH "-1")
	message(STATUS 
		"Building from a new commit detected, \
		the build number will be set to 1.")
endif()

if(SHORT_HASH STREQUAL OLD_HASH AND 
	EXISTS "${PROJECT_BINARY_DIR}/config/BuildNumber.txt")
	file (STRINGS "${PROJECT_BINARY_DIR}/config/BuildNumber.txt" BUILD_NUMBER)
	math(EXPR BUILD_NUMBER "${BUILD_NUMBER}+1")
else()
	set(BUILD_NUMBER "1")
	file (WRITE "${PROJECT_BINARY_DIR}/config/OldGitHash.txt" "${SHORT_HASH}")
endif()

file (WRITE "${PROJECT_BINARY_DIR}/config/BuildNumber.txt" "${BUILD_NUMBER}")

msg(ATTENTION 
	"[CMAKE CONFIG VERSION:\t${CMAKE_PROJECT_VERSION}]")
msg(ATTENTION 
	"[DEBUG BUILD VERSION:\t${TODAY}-${NOW}-${BUILD_NUMBER}-${SHORT_HASH}]")
msg(ATTENTION 
	"[RELEASE BUILD VERSION:\t${TODAY}-${NOW}-${BUILD_NUMBER}-${SHORT_HASH}]")

# MAIN PROJECT SETTINGS #######################################################

# Only do these if this is the main project, 
# and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

	# Optionally set things like CMAKE_CXX_STANDARD, 
	# CMAKE_POSITION_INDEPENDENT_CODE here
	
	# Let's ensure -std=c++xx instead of -std=g++xx
	set(CMAKE_CXX_EXTENSIONS OFF)

	# Let's nicely support folders in IDE's
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)

	# Testing only available if this is the main app
	include(CTest)

	if(BUILD_WARDROBE_DOC)
		# Docs only available if this is the main app
		find_package(Doxygen 
			REQUIRED dot 
			OPTIONAL_COMPONENTS mscgen dia)

		if(NOT DOXYGEN_FOUND)
			message(FATAL_ERROR "Doxygen is needed to build the documentation")
		endif()

		if(DOXYGEN_FOUND)
			message(STATUS "Doxygen found, creating target started")

			set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")

			get_property(
				IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

			#CMAKE_CONFIGURATION_TYPES
			if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug" OR IS_MULTI_CONFIG)
				set(DOXYGEN_GENERATE_HTML  YES)
				set(DOXYGEN_GENERATE_LATEX NO)
				set(DOXYGEN_GENERATE_MAN   NO)

				# DOXYGEN_PROJECT_NAME PROJECT_NAME
				# DOXYGEN_PROJECT_NUMBER PROJECT_VERSION
				# DOXYGEN_PROJECT_BRIEF PROJECT_DESCRIPTION
				# DOXYGEN_OUTPUT_DIRECTORY
				# Set to CMAKE_CURRENT_BINARY_DIR by this module. 

				set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs")

				doxygen_add_docs(
					documentation
					"${PROJECT_SOURCE_DIR}/README.md"
					"${PROJECT_SOURCE_DIR}/game"
					"${PROJECT_SOURCE_DIR}/libs"
					"${PROJECT_SOURCE_DIR}/config"
					COMMENT "Generating API documentation with Doxygen"
				)
			endif()
		else()
			message(STATUS "Doxygen not found, not building docs")
		endif()
	endif()
endif()

# SUBDIRECTORIES ##############################################################

# thirdparty libraries
add_subdirectory(thirdparty)

# internal libraries
add_subdirectory(libs)

# main executable
add_subdirectory(game)

if(MSVC)
	# set the starting project in the generated solution to `game`
	set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" 
		PROPERTY VS_STARTUP_PROJECT game)
endif(MSVC)

# CLANG-TIDY ##################################################################

# if(CMAKE_VERSION VERSION_GREATER 3.6)
#     # Add clang-tidy if available
#     option(CLANG_TIDY_FIX "Perform fixes for Clang-Tidy" OFF)
#     find_program(
#         CLANG_TIDY_EXE
#         NAMES "clang-tidy"
#         DOC "Path to clang-tidy executable"
#     )

# 	message(STATUS "###[CLANG-TIDY]### -----------------")

#     if(CLANG_TIDY_EXE)
# 	    message(STATUS "Clang-tidy found: ${CLANG_TIDY_EXE}")

#         if(CLANG_TIDY_FIX)
#             set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}" "-fix")
#         else()
#             set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
#         endif()
#     endif()
# endif()

# PREPARE COMPILED EXECUTABLE, DLLS, AND ASSETS ###############################

include(InstallRequiredSystemLibraries)

# set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE)
install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION "bin" COMPONENT "game")

install(TARGETS game RUNTIME DESTINATION "bin" COMPONENT "game")
install(TARGETS openal-info RUNTIME DESTINATION "bin" COMPONENT "game")
install(TARGETS OpenAL RUNTIME DESTINATION "bin" COMPONENT "game")

# Copy all assets to resources file
install(
	DIRECTORY "${CMAKE_SOURCE_DIR}/assets/" 
	DESTINATION "bin/assets"
	COMPONENT "game")

# Copy settings.toml to the game install directory
install(
	FILES "${CMAKE_SOURCE_DIR}/settings/DefaultSettings.toml" 
	DESTINATION "bin"
	COMPONENT "game"
	RENAME "settings.toml")

# PACKAGING ###################################################################

# CPACK ### METADATA ##########################################################

# on Windows it will decide the name of Program Files install directory
set(CPACK_PACKAGE_INSTALL_DIRECTORY "WARdrobe")

set(CPACK_PACKAGE_NAME "WARdrobe")
set(CPACK_PACKAGE_DESCRIPTION "WARdrobe: Self-Assembly")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
	"MyLib - CPack Component Installation Example")
set(CPACK_PACKAGE_VENDOR "ACME")
set(CPACK_PACKAGE_CONTACT "support@acme.corp")

if(CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug" AND NOT IS_MULTI_CONFIG)
	set(DEBUG_STRING "-debug")
elseif(NOT DEFINED DEBUG_STRING)
	set(DEBUG_STRING "")
endif()

if(NOT DEFINED CPACK_PACKAGE_FILE_NAME)
	set(INSTALLER_VERSION_STRING
		"${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${DEBUG_STRING}")

	set(CPACK_PACKAGE_FILE_NAME 
		"${CMAKE_PROJECT_NAME}-${INSTALLER_VERSION_STRING}")
endif()

set(CPACK_COMPONENTS_ALL game)

# CPACK ### WIX-SPECIFIC SETTINGS #############################################
 
# allow installers to replace existing installations that use the same GUID
set(CPACK_WIX_UPGRADE_GUID "35367698-CDE9-4AC4-8BCA-9240DC7A71AF")

set(CPACK_WIX_UNINSTALL "1")

# CPACK ### NSIS-SPECIFIC SETTINGS ############################################

# An icon filename. The name of a *.ico file used as the main icon 
# for the generated install program.
set(CPACK_NSIS_MUI_ICON 
	"${CMAKE_SOURCE_DIR}/resources/images/icons/wardrobe.ico")

# An icon filename. The name of a *.ico file used as the main icon 
# for the generated uninstall program.
set(CPACK_NSIS_MUI_UNIICON 
	"${CMAKE_SOURCE_DIR}/resources/images/icons/wardrobe.ico")

# The display name string that appears
# in the Windows Add/Remove Program control panel
set(CPACK_NSIS_DISPLAY_NAME "Wardrobe NSIS test")

# The title displayed at the top of the installer.
# on Windows it will be displayed in All Applications list as a folder
set(CPACK_NSIS_PACKAGE_NAME "WARdrobe")

# CPACK ### SELECT THE INSTALLER BUILDER ######################################

if(NOT CPACK_GENERATOR)
	if (WIN32)
		if(USE_WIX_TOOLSET)
			# this need WiX Tooset installed and a path to candle.exe
			set(CPACK_GENERATOR "WIX")
		else()
			# this needs NSIS installed, and available
			set(CPACK_GENERATOR "NSIS")
		endif ()
		set(CPACK_SOURCE_GENERATOR "ZIP")
	elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(CPACK_GENERATOR "PackageMake")
	else()
		set(CPACK_GENERATOR "DEB")
		set(CPACK_SOURCE_GENERATOR "TGZ")
	endif()
endif()

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Michał K. Kotowski") #required!

# CPACK ### OUTPUT SETTINGS ###################################################

set(CPACK_OUTPUT_CONFIG_FILE "CPackConfig.cmake")
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/packages")

# CPACK ### PACKAGE VERSION ###################################################

set(CPACK_PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")

# CPACK ### RESOURCE FILES ####################################################

# set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.html")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
# set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/README.html")

set(CPACK_SOURCE_IGNORE_FILES 
	"${CMAKE_SOURCE_DIR}/build/;${CMAKE_SOURCE_DIR}/.git/")

set(CPACK_PACKAGE_EXECUTABLES 
	"0wardrobe" "WARdrobe Self-Assembly")

set(CPACK_CREATE_DESKTOP_LINKS 
	"0wardrobe")

# CPACK ### CHECKSUM GENERATION ###############################################

set(CPACK_PACKAGE_CHECKSUM "MD5")

# CPACK ### INCLUDE CPACK AFTER SETTINGS ######################################

include(CPack)

# TARGETS FOR GENERATING MULTIPLE INSTALLERS AT ONCE ##########################

if(WIN32 AND MSVC)

	# ADD_CUSTOM_TARGET(
	# windows_debug_packages
	# 	COMMAND ${CMAKE_COMMAND} 
	# 		${CMAKE_SOURCE_DIR} 
	# 		-G "Visual Studio 16 2019" 
	# 		-DINCLUDE_DEBUG_UI=ON
	# 		-DDEBUG_STRING="-debug"
	# 		-DASSIMP_BUILD_ZLIB=TRUE
	# 		-DBUILD_LIB_TYPE="STATIC"
	# 		-DBUILD_SHARED_LIBS=FALSE
	# 	COMMAND ${CMAKE_COMMAND} 
	# 		-E remove_directory ${CMAKE_INSTALL_PREFIX}
	# 	COMMAND ${CMAKE_COMMAND} 
	# 		--build ${CMAKE_BINARY_DIR} 
	# 		--target ALL_BUILD 
	# 		--config Debug
	# 	COMMAND ${CMAKE_COMMAND} 
	# 		-P cmake_install.cmake
	# 	COMMAND cpack 
	# 		--config ${CMAKE_BINARY_DIR}/CPackConfig.cmake 
	# 		-C Debug 
	# 		-G ZIP
	# 	COMMAND cpack 
	# 		--config ${CMAKE_BINARY_DIR}/CPackConfig.cmake 
	# 		-C Debug 
	# 		-G WIX
	# 	COMMAND cpack 
	# 		--config ${CMAKE_BINARY_DIR}/CPackConfig.cmake 
	# 		-C Debug 
	# 		-G NSIS
	# )

	ADD_CUSTOM_TARGET(
	windows_release_with_debug_tools_packages
		COMMAND ${CMAKE_COMMAND} 
			${CMAKE_SOURCE_DIR} 
			-G "Visual Studio 16 2019" 
			-DINCLUDE_DEBUG_UI=ON 
			-DHIDE_CRT_WIN32_MSVC=OFF
			-DDEBUG_STRING="-rc" # release candidate
			-DASSIMP_BUILD_ASSIMP_TOOLS=OFF
		COMMAND ${CMAKE_COMMAND} 
			-E remove_directory ${CMAKE_INSTALL_PREFIX}
		COMMAND ${CMAKE_COMMAND} 
			--build ${CMAKE_BINARY_DIR} 
			--target ALL_BUILD 
			--config Release
		COMMAND ${CMAKE_COMMAND} 
			-P cmake_install.cmake
		COMMAND cpack 
			--config ${CMAKE_BINARY_DIR}/CPackConfig.cmake 
			-C Release 
			-G ZIP
		COMMAND cpack 
			--config ${CMAKE_BINARY_DIR}/CPackConfig.cmake 
			-C Release 
			-G WIX
		COMMAND cpack 
			--config ${CMAKE_BINARY_DIR}/CPackConfig.cmake 
			-C Release 
			-G NSIS
	)

	ADD_CUSTOM_TARGET(
	windows_release_packages
		COMMAND ${CMAKE_COMMAND} 
			${CMAKE_SOURCE_DIR} 
			-G "Visual Studio 16 2019" 
			-DINCLUDE_DEBUG_UI=OFF 
			-DHIDE_CRT_WIN32_MSVC=ON
			-DDEBUG_STRING=""
		COMMAND ${CMAKE_COMMAND} 
			-E remove_directory ${CMAKE_INSTALL_PREFIX}
		COMMAND ${CMAKE_COMMAND} 
			--build ${CMAKE_BINARY_DIR} 
			--target ALL_BUILD 
			--config Release
		COMMAND ${CMAKE_COMMAND} 
			-P cmake_install.cmake
		COMMAND cpack 
			--config ${CMAKE_BINARY_DIR}/CPackConfig.cmake 
			-C Release 
			-G ZIP
		COMMAND cpack 
			--config ${CMAKE_BINARY_DIR}/CPackConfig.cmake 
			-C Release 
			-G WIX
		COMMAND cpack 
			--config ${CMAKE_BINARY_DIR}/CPackConfig.cmake 
			-C Release 
			-G NSIS
	)

	# set_target_properties (windows_debug_packages PROPERTIES
	# 	FOLDER packages
	# )

	set_target_properties (windows_release_with_debug_tools_packages PROPERTIES
		FOLDER packages
	)

	set_target_properties (windows_release_packages PROPERTIES
		FOLDER packages
	)

endif(WIN32 AND MSVC)