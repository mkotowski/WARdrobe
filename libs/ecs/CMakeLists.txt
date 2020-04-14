project(ecs VERSION 0.1.0 LANGUAGES CXX)

# LIST HEADER AND SOURCE FILES ################################################

file(GLOB HEADER_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

file(GLOB COFIGURED_HEADERS CONFIGURE_DEPENDS 
	"${CMAKE_BINARY_DIR}/config/*.h")

add_library(ecs STATIC ${SOURCE_LIST} ${HEADER_LIST} ${COFIGURED_HEADERS})

# TARGET SETTINGS #############################################################

target_include_directories(ecs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_features(ecs PUBLIC cxx_std_17)
