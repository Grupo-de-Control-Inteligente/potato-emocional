cmake_minimum_required(VERSION 2.8.3)
project(ModeloEmocional)

###########
## Build ##
###########

## Specify additional locations of header files

set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

## Your package locations should be listed before other locations
find_package(PkgConfig REQUIRED)
include_directories(include/)
link_directories(lib/)



## Declare a cpp executable
file(GLOB_RECURSE ModeloEmocional_SOURCES ModeloEmocional.cpp)
#file(GLOB_RECURSE ModeloEmocional_HEADERS include/*.h)

#set(ModeloEmocional_INCLUDE_DIRS "")
#foreach(_headerFile ${ModeloEmocional_HEADERS})
	#MESSAGE("Now Globbing.... ${ModeloEmocional_HEADERS}")
#	get_filename_component(_dir ${_headerFile} PATH)
#	list(APPEND ModeloEmocional_INCLUDE_DIRS ${_dir})
#endforeach()
#list(REMOVE_DUPLICATES ModeloEmocional_INCLUDE_DIRS)

#include_directories(${ModeloEmocional_INCLUDE_DIRS})
add_executable(ModeloEmocional 
	${ModeloEmocional_SOURCES}
)

## Add cmake target dependencies of the executable/library
## as an example, message headers may need to be generated before nodes
# add_dependencies(ModeloEmocional ModeloEmocional_generate_messages_cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(ModeloEmocional
  pthread rt
  fuzzylite
  dl
  lcm
)

set_target_properties(ModeloEmocional PROPERTIES COMPILE_FLAGS "-Wall")
set_target_properties(ModeloEmocional PROPERTIES COMPILE_FLAGS "-m32")
set_target_properties(ModeloEmocional PROPERTIES COMPILE_FLAGS "-Wno-write-strings")
set_target_properties(ModeloEmocional PROPERTIES COMPILE_FLAGS "-fPIC")
#set_target_properties(ModeloEmocional PROPERTIES LINK_FLAGS "-Wl")