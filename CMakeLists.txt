cmake_minimum_required(VERSION 3.10)
project(todo)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SRC_FILES "./*.c")

set(BIN_DIR "${CMAKE_SOURCE_DIR}/bin")
file(MAKE_DIRECTORY ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

add_executable(todo ${SRC_FILES})

if(WIN32)
  target_include_directories(todo
  PRIVATE
    C:/lib/cJSON/include
  )

  target_link_libraries(todo
    C:/lib/cJSON/lib/libcjson.a
  )
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
elseif(UNIX)
  target_link_libraries(todo cjson)
endif()

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
