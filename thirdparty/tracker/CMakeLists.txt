set(TARGET_NAME "MOT_Tracker")

file (GLOB MAIN_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
        )


include_directories (${OpenCV_INCLUDE_DIRS} )

add_library(${TARGET_NAME} SHARED ${MAIN_SRC} ${MAIN_HEADERS})
add_dependencies(${TARGET_NAME} ${third_party_deps})
target_link_libraries(${TARGET_NAME} ${OPENCV_LIBS})




