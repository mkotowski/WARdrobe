project(input VERSION 0.1.0 LANGUAGES CXX C)

# LIST HEADER AND SOURCE FILES ################################################

file(GLOB HEADER_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

file(GLOB COFIGURED_HEADERS CONFIGURE_DEPENDS 
	"${CMAKE_BINARY_DIR}/config/*.h")

add_library(input STATIC ${SOURCE_LIST} ${HEADER_LIST} ${COFIGURED_HEADERS})
set_target_properties(input PROPERTIES LINKER_LANGUAGE CXX)

# TARGET SETTINGS #############################################################

target_include_directories(input PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    "${CMAKE_BINARY_DIR}/config/"
)

target_compile_features(input PUBLIC cxx_std_17)

