set(CMAKE_POLICY_DEFAULT_CMP0054 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0053 NEW)

set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

msg(STATUS "[CONFIGURE EXTERNAL PROJECTS - START]")

# GLAD ########################################################################

msg(HEADER "[ltjax/steam_controller - support without Steam Input API]")

add_subdirectory(steam_controller)

set(STEAM_CONTROLLER_DIR "${THIRDPARTY_DIR}/steam_controller")
set(STEAM_CONTROLLER_LIBRARY "steam_controller")
set(STEAM_CONTROLLER_INCLUDE_DIR "${STEAM_CONTROLLER_DIR}/include")

set_property(GLOBAL PROPERTY 
	STEAM_CONTROLLER_DIR "${THIRDPARTY_DIR}/steam_controller")
set_property(GLOBAL PROPERTY 
	STEAM_CONTROLLER_INCLUDE_DIR "${STEAM_CONTROLLER_DIR}/include")

set_target_properties (steam_controller PROPERTIES
	FOLDER thirdparty/steam_controller
)

set_target_properties (demo PROPERTIES
	FOLDER thirdparty/steam_controller
)

# GLAD ########################################################################

msg(HEADER "[GLAD]")

# generated from https://glad.dav1d.de/
# Language:      C/C++
# Specification: OpenGL
# gl:            Version 3.3
# Profile:       Core
# Extensions:    ==NONE==

set(GLAD_DIR "${THIRDPARTY_DIR}/glad")
set(GLAD_LIBRARY "glad")
set(GLAD_INCLUDE_DIR "${GLAD_DIR}/include")

set_property(GLOBAL PROPERTY GLAD_DIR "${THIRDPARTY_DIR}/glad")
set_property(GLOBAL PROPERTY GLAD_INCLUDE_DIR "${GLAD_DIR}/include")

add_library("glad" STATIC 
	"${GLAD_INCLUDE_DIR}/KHR/khrplatform.h"
	"${GLAD_INCLUDE_DIR}/glad/glad.h"
	"${GLAD_DIR}/src/glad.c"
)

target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")

set_target_properties (glad PROPERTIES
	FOLDER thirdparty/glad
)

# ASSIMP ######################################################################

msg(HEADER "[ASSIMP]")

set(BUILD_SHARED_LIBS OFF CACHE BOOL 
	"Build package with shared libraries." 
	FORCE)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL 
	"If the test suite for Assimp is built in addition to the library."
	FORCE)

set(ASSIMP_INSTALL OFF CACHE BOOL 
	"Disable this if you want to use assimp as a submodule." 
	FORCE)

add_subdirectory(assimp)

set(ASSIMP_INCLUDE_DIR "${THIRDPARTY_DIR}/assimp")

set_target_properties (assimp PROPERTIES
	FOLDER thirdparty/assimp
)

set_target_properties (assimp_cmd PROPERTIES
	FOLDER thirdparty/assimp
)

set_target_properties (IrrXML PROPERTIES
	FOLDER thirdparty/assimp
)

set_target_properties (uninstall PROPERTIES
	FOLDER thirdparty/assimp
)

if(ASSIMP_BUILD_TESTS)
	set_target_properties (unit PROPERTIES
		FOLDER thirdparty/assimp
	)
endif(ASSIMP_BUILD_TESTS)

set_target_properties (zlib PROPERTIES
	FOLDER thirdparty/assimp
)

set_target_properties (zlibstatic PROPERTIES
	FOLDER thirdparty/assimp
)

set_target_properties (UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES
	FOLDER thirdparty/assimp
)

# GLFW ########################################################################

msg(HEADER "[GLFW]")

set(GLFW_USE_HYBRID_HPG OFF CACHE INTERNAL "" FORCE)
set(GLFW_VULKAN_STATIC  OFF CACHE INTERNAL "" FORCE)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS    OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS     OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL        OFF CACHE INTERNAL "Generate installation target")

set(GLFW_DIR "${THIRDPARTY_DIR}/glfw")
set(GLFW_LIBRARY "glfw" "${GLFW_LIBRARIES}")

set_property(GLOBAL PROPERTY GLFW_INCLUDE_DIR "${GLFW_DIR}/include")

add_subdirectory("${GLFW_DIR}")

set_target_properties (glfw PROPERTIES
	FOLDER thirdparty/glfw
)

# OpenGL Mathematics (GLM) ####################################################

msg(HEADER "[GLM]")

add_subdirectory(glm)

set_property(GLOBAL PROPERTY GLM_DIR "${THIRDPARTY_DIR}/glm")
set_property(GLOBAL PROPERTY GLM_INCLUDE_DIR "${GLM_DIR}")

# Dear ImGUI ##################################################################

msg(HEADER "[IMGUI]")

file(GLOB IMGUI_SOURCE ./imgui/*.cpp)
file(GLOB IMGUI_HEADER ./imgui/*.h)

add_library(imgui STATIC 
	${IMGUI_SOURCE}
	${IMGUI_HEADER}
	"${THIRDPARTY_DIR}/imgui/examples/imgui_impl_opengl3.cpp"
	"${THIRDPARTY_DIR}/imgui/examples/imgui_impl_opengl3.h"
	"${THIRDPARTY_DIR}/imgui/examples/imgui_impl_glfw.cpp"
	"${THIRDPARTY_DIR}/imgui/examples/imgui_impl_glfw.h")

set(IMGUI_DIR "${THIRDPARTY_DIR}/imgui")
set(IMGUI_LIBRARY "imgui")
set(IMGUI_INCLUDE_DIR "${IMGUI_DIR}")

set_property(GLOBAL PROPERTY IMGUI_INCLUDE_DIR "${IMGUI_INCLUDE_DIR}")

include_directories(SYSTEM "${THIRDPARTY_DIR}")
include_directories(SYSTEM "${THIRDPARTY_DIR}/imgui/")
include_directories(SYSTEM "${THIRDPARTY_DIR}/imgui/examples")

target_include_directories(imgui PRIVATE 
	"${THIRDPARTY_DIR}/imgui/"
	"${THIRDPARTY_DIR}/imgui/examples/"
	"${GLAD_INCLUDE_DIR}"
	"${GLFW_INCLUDE_DIR}"
)

target_link_libraries(imgui PRIVATE glad glfw)

set_target_properties (imgui PROPERTIES
	FOLDER thirdparty/imgui
)

# OPENAL-SOFT #################################################################

msg(HEADER "[OPENAL-SOFT]")

set_property(GLOBAL PROPERTY OPENAL_DIR "${THIRDPARTY_DIR}/openal")
set_property(GLOBAL PROPERTY OPENAL_INCLUDE_DIR "${OPENAL_DIR}/include")

set(ALSOFT_EXAMPLES OFF CACHE BOOL 
	"Build and install example programs" 
	FORCE)

set(ALSOFT_TESTS OFF CACHE BOOL 
	"Build and install test programs" 
	FORCE)

add_subdirectory(openal)

set_target_properties (build_version PROPERTIES
	FOLDER thirdparty/openal
)

set_target_properties (common PROPERTIES
	FOLDER thirdparty/openal
)

set_target_properties (ex-common PROPERTIES
	FOLDER thirdparty/openal
)

if (TARGET native-tools)
	set_target_properties (native-tools PROPERTIES
		FOLDER thirdparty/openal
	)
endif()

set_target_properties (OpenAL PROPERTIES
	FOLDER thirdparty/openal
)

set_target_properties (openal-info PROPERTIES
	FOLDER thirdparty/openal
)

set_target_properties(openal-info PROPERTIES OUTPUT_NAME "openal-info")

# NLOHMANN JSON ###############################################################

msg(HEADER "[NLOHMANN JSON]")

set(JSON_BuildTests OFF CACHE INTERNAL 
	"Build the unit tests when BUILD_TESTING is enabled." 
	FORCE)

set(JSON_Install OFF CACHE INTERNAL 
	".." 
	FORCE)

set(JSON_MultipleHeaders OFF CACHE INTERNAL 
	".." 
	FORCE)

add_subdirectory(json)

# SKYSTRIFE CPPTOML ###########################################################

msg(HEADER "[SKYSTRIFE CPPTOML]")

set(CPPTOML_BUILD_EXAMPLES OFF CACHE INTERNAL 
	"Build examples" 
	FORCE)

# an empty library to override automatic doc
# generation from cpptoml's cmakelists
add_library(doc INTERFACE)

add_subdirectory(cpptoml)

set_property(GLOBAL PROPERTY 
	CPPTOML_INCLUDE_DIR "${THIRDPARTY_DIR}/cpptoml/include")

# LUA #########################################################################

msg(HEADER "[LUA]")

add_subdirectory(lua)

set_target_properties (lua PROPERTIES
	FOLDER thirdparty/lua
)

# stb_image ###################################################################

msg(HEADER "[STB_IMAGE]")

set(STB_IMAGE_DIR "${THIRDPARTY_DIR}/stb_image")

add_library("stb_image" STATIC 
	"${STB_IMAGE_DIR}/stb_image.cpp" 
	"${STB_IMAGE_DIR}/stb_image.h")

target_include_directories("stb_image" PRIVATE "${STB_IMAGE_DIR}")

set(STB_IMAGE_LIBRARY "stb_image")
set(STB_IMAGE_INCLUDE_DIR "${STB_IMAGE_DIR}")

set_property(GLOBAL PROPERTY 
	STB_IMAGE_DIR "${STB_IMAGE_DIR}")
set_property(GLOBAL PROPERTY 
	STB_IMAGE_INCLUDE_DIR "${STB_IMAGE_DIR}")


set_target_properties (stb_image PROPERTIES
	FOLDER thirdparty/stb_image
)

msg(STATUS "[CONFIGURE EXTERNAL PROJECTS - END]")