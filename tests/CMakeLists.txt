SET(${CXX_STANDARD_REQUIRED} ON)

LINK_DIRECTORIES(../dll/x64/Release)
LINK_DIRECTORIES("C:/Program Files (x86)/Windows Kits/10/Lib/10.0.16299.0/um/x64")
#LINK_DIRECTORIES("C:/Program Files (x86)/Windows Kits/10/Redist/D3D/x64")


INCLUDE_DIRECTORIES(../dependencies)
INCLUDE_DIRECTORIES(../src)
INCLUDE_DIRECTORIES(../dependencies/gtest/googletest/include/)

GroupSources(tests)

#ADD_EXECUTABLE(EngineTest WIN32 EngineTest.cpp)
#TARGET_LINK_LIBRARIES(EngineTest partition)
#SET_PROPERTY(TARGET EngineTest PROPERTY CXX_STANDARD 17)

# ==========================================================================
file(GLOB_RECURSE TEST_SOURCES */*.cpp)
file(GLOB_RECURSE TEST_HEADERS */*.h)

# GtestMain.cpp
ADD_EXECUTABLE(Tests ${TEST_SOURCES} ${TEST_HEADERS})
ADD_DEPENDENCIES(Tests gtest gtest_main)
TARGET_LINK_LIBRARIES(Tests partition gtest gtest_main)
SET_PROPERTY(TARGET Tests PROPERTY CXX_STANDARD 17)

