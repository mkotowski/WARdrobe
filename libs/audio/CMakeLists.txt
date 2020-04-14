project(audio)

file(GLOB HEADER_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

include_directories("${PROJECT_BINARY_DIR}/config")

get_property(COFIGURED_HEADERS GLOBAL PROPERTY COFIGURED_HEADERS)
get_property(OPENAL_INCLUDE_DIR GLOBAL PROPERTY OPENAL_INCLUDE_DIR)

add_library(audio STATIC
	${SOURCE_LIST}
	${HEADER_LIST}
	${COFIGURED_HEADERS}
)

target_include_directories(audio 
	PUBLIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/"
	"${OPENAL_INCLUDE_DIR}"
	"${CMAKE_SOURCE_DIR}/libs/assetmanager/include"
)

# This depends on (header only) boost
target_link_libraries(audio PUBLIC OpenAL assetmanager)

# All users of this library will need at least C++11
target_compile_features(audio PUBLIC cxx_std_17)
