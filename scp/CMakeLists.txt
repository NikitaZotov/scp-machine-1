set(SCP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.hpp" "src/*.cpp"
    "src/*/*.hpp" "src/*/*.cpp"
)

add_definitions(-DSCP_DEBUG)
add_definitions(-DSCP_MATH)
add_definitions(-DSCP_STRING)

find_package(sc-machine REQUIRED)

add_library(scp SHARED ${SOURCES} ${HEADERS})
target_link_libraries(scp LINK_PUBLIC sc-machine::sc-memory)
target_include_directories(scp PRIVATE ${SCP_SRC})
set_target_properties(scp PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})

if (${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(scp)
endif()

if (${SC_BUILD_TESTS})
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
endif ()
