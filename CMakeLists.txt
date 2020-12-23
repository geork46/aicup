cmake_minimum_required(VERSION 3.5)
project(aicup2020)

# OS and compiler checks.
if(WIN32)
    add_definitions(-DWIN32)
    SET(PROJECT_LIBS Ws2_32.lib)
endif()

if (DEFINED ENV{gui-test})
    add_definitions(-DGUI_TEST)
    message("gui-test")
    file(GLOB HEADERS2 "../gui-test/*.h")
    file(GLOB SRC2 "../gui-test/*.cpp")
    include_directories(../gui-test/include/)
    include_directories(../gui-test/)
    file(GLOB MYLIB ../gui-test/libcsimplesocket.a )

endif()

file(GLOB HEADERS "*.hpp" "*.h" "model/*.hpp")
SET_SOURCE_FILES_PROPERTIES(${HEADERS} PROPERTIES HEADER_FILE_ONLY TRUE)
file(GLOB SRC "*.cpp" "model/*.cpp")
add_executable(aicup2020 ${HEADERS} ${SRC} ${SRC2} ${HEADERS2})
TARGET_LINK_LIBRARIES(aicup2020 ${PROJECT_LIBS} ${MYLIB})

