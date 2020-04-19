project(renderer VERSION 0.1.0 LANGUAGES CXX)

# LIST HEADER AND SOURCE FILES ################################################

file(GLOB HEADER_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

file(GLOB COFIGURED_HEADERS CONFIGURE_DEPENDS 
	"${CMAKE_BINARY_DIR}/config/*.h")

add_library(renderer STATIC ${SOURCE_LIST} ${HEADER_LIST} ${COFIGURED_HEADERS})

# GET INCLUDE DIRS OF USED LIBRARIES ##########################################

get_property(GLFW_INCLUDE_DIR  GLOBAL PROPERTY  GLFW_INCLUDE_DIR)
get_property(GLAD_INCLUDE_DIR  GLOBAL PROPERTY  GLAD_INCLUDE_DIR)
get_property(IMGUI_INCLUDE_DIR GLOBAL PROPERTY IMGUI_INCLUDE_DIR)

# TARGET SETTINGS #############################################################

get_property(STB_IMAGE_INCLUDE_DIR GLOBAL PROPERTY STB_IMAGE_INCLUDE_DIR)

target_include_directories(renderer PUBLIC
    "${CMAKE_BINARY_DIR}/thirdparty/assimp/include" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    "${CMAKE_SOURCE_DIR}/thirdparty/assimp/include/"
    "${GLAD_INCLUDE_DIR}"
    "${GLFW_INCLUDE_DIR}"
    "${IMGUI_INCLUDE_DIR}"
    "${IMGUI_INCLUDE_DIR}/examples/"
    "${CMAKE_BINARY_DIR}/config/"
    "${STB_IMAGE_INCLUDE_DIR}"
)

target_link_libraries(renderer PRIVATE glad glfw glm imgui assimp assetmanager)

target_compile_features(renderer PUBLIC cxx_std_17)
