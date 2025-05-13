cmake_minimum_required(VERSION 3.16)
project(objectbox_ts_demo)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
include_directories(src)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/ts-data-model.obx.cpp
        src/util/StopWatch.cpp
)

# Fetch ObjectBox for the includes, ObjectBox Generator and the non-TS library (as a fallback)
include(FetchContent)
#set(ObjectBoxGenerator_CMAKE_VERSION dev)
FetchContent_Declare(
        objectbox
        GIT_REPOSITORY https://github.com/objectbox/objectbox-c.git
        GIT_TAG v4.3.0
)
FetchContent_MakeAvailable(objectbox)

# By fetching ObjectBox, we can also use the ObjectBoxGenerator package via CMake:
find_package(ObjectBoxGenerator REQUIRED)
add_obx_schema(TARGET ${PROJECT_NAME}
        SCHEMA_FILES ts-data-model.fbs
        CXX_STANDARD 11
        INSOURCE
        OUTPUT_DIR src
        OUTPUT_DIR_MODEL_JSON .
)

# Put the time series enabled library here: lib/libobjectbox.so and reload the CMake project.
# If it exists, link it as objectbox-ts; otherwise link the standard objectbox from FetchContent.
if (EXISTS ${CMAKE_SOURCE_DIR}/lib/libobjectbox.so)
    message(STATUS "Using the ObjectBox library in lib/ (hopefully time series enabled)")
    link_directories(lib)
    add_library(objectbox-ts SHARED IMPORTED)
    set_target_properties(objectbox-ts PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libobjectbox.so )
    target_link_libraries(${PROJECT_NAME} objectbox-ts)
else ()
    message(WARNING "No ObjectBox library found in lib/ (put the time series enabled library there)")
    target_link_libraries(${PROJECT_NAME} objectbox)
endif ()
