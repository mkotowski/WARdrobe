project(assetmanager VERSION 0.1.0 LANGUAGES CXX)

# LIST HEADER AND SOURCE FILES ################################################

file(GLOB HEADER_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

file(GLOB COFIGURED_HEADERS CONFIGURE_DEPENDS 
	"${CMAKE_BINARY_DIR}/config/*.h")

add_library(assetmanager STATIC ${SOURCE_LIST} ${HEADER_LIST} ${COFIGURED_HEADERS})

# GET INCLUDE DIRS OF USED LIBRARIES ##########################################

get_property(CPPTOML_INCLUDE_DIR GLOBAL PROPERTY CPPTOML_INCLUDE_DIR)

# TARGET SETTINGS #############################################################

target_include_directories(assetmanager PUBLIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/"
	"${CMAKE_BINARY_DIR}/config/"
	"${CPPTOML_INCLUDE_DIR}"
)

target_link_libraries(assetmanager PRIVATE cpptoml)

target_compile_features(assetmanager PUBLIC cxx_std_17)
