##########################################################
###                             UMapDisplayer
##########################################################
cmake_minimum_required(VERSION 3.0)

project(undustpics-cv)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED program_options filesystem)

### Project description.
add_executable(undustpics-cv main.cpp)
include_directories(undustpics-cv ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(undustpics-cv ${OpenCV_LIBS} ${Boost_LIBRARIES})

