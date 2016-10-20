#--------------------------------------------------------------------------
# EDE project
#--------------------------------------------------------------------------


cmake_minimum_required(VERSION 2.8.11)

project(EDE)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
#Runs UIC if ui_XXX.h is included in headers and expects XXX.ui
SET(CMAKE_AUTOUIC ON)
#QT Resource files
SET(CMAKE_AUTORCC ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Qt5Widgets REQUIRED)

INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDES})			

FILE(GLOB HEAD "code/*.h")
FILE(GLOB SRC "code/*.cpp")
LIST(APPEND CODE ${HEAD} ${SRC})
SOURCE_GROUP("code" FILES ${CODE})

FILE(GLOB UI "code/*.ui")
SOURCE_GROUP("ui" FILES ${UI})

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
ADD_EXECUTABLE(EDE WIN32 ${CODE} ${UI})

TARGET_LINK_LIBRARIES(EDE Qt5::Widgets)

ADD_CUSTOM_COMMAND(TARGET EDE PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E make_directory
						${CMAKE_CURRENT_SOURCE_DIR}/bin)