﻿file(GLOB_RECURSE GAME_SOURCE *.cpp)
file(GLOB_RECURSE GAME_HEADERS *.h)

SET(${CXX_STANDARD_REQUIRED} ON)

LINK_DIRECTORIES(../dll/x64/Release)
LINK_DIRECTORIES("C:/Program Files (x86)/Windows Kits/10/Lib/10.0.16299.0/um/x64")

INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(../dependencies)
INCLUDE_DIRECTORIES(../src)

ADD_EXECUTABLE(Protogame WIN32 ${GAME_SOURCE} ${GAME_HEADERS})
TARGET_LINK_LIBRARIES(Protogame Partition)
SET_PROPERTY(TARGET Protogame PROPERTY CXX_STANDARD 17)
SET_PROPERTY(TARGET Protogame PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${exampleName}")