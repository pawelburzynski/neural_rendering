cmake_minimum_required (VERSION 2.8.11)
project (Practical_2)

set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

find_package (OpenCL)
if( NOT OpenCL_FOUND )
    MESSAGE( SEND_ERROR "OpenCL not found. Install OpenCL development files." )
endif( NOT OpenCL_FOUND )

find_package(Qt5 COMPONENTS Widgets OpenGL)
if(NOT Qt5Widgets_FOUND)
      MESSAGE( SEND_ERROR "Qt5 library not found." )
endif(NOT Qt5Widgets_FOUND)

include_directories ("${PROJECT_SOURCE_DIR}/../include" ${OpenCL_INCLUDE_DIRS})

add_subdirectory (task0) 
add_subdirectory (task1)
add_subdirectory (task2)
add_subdirectory (task3)
add_subdirectory (task4)
add_subdirectory (task5)
