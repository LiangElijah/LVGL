CMAKE_MINIMUM_REQUIRED(VERSION 3.16.3)
PROJECT(
    app 
    VERSION 
    1.0 
    DESCRIPTION "common application" 
    LANGUAGES C
)

AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/src SOURCES)
INCLUDE_DIRECTORIES(
    ./include 
    ../Git/lvgl/out/include
)
LINK_DIRECTORIES(
    ../Git/lvgl/out/lib
)
LINK_LIBRARIES(lvgl)
ADD_COMPILE_OPTIONS(-Wno-packed-bitfield-compat)

ADD_EXECUTABLE(main ${SOURCES})

SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_CROSSCOMPILING True)
SET(CMAKE_C_COMPILER arm-none-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER arm-none-linux-gnueabihf-g++)