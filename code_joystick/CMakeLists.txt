cmake_minimum_required(VERSION 2.6)
# set project's name
project(MARSI_CONTROL)

# with SET() command you can change variables or define new ones
# here we define SAMPLE_SRCS variable that contains a list of all .cpp files
# note that we don't need \ at the end of line
SET(SOURCES
    src/MainWindow.cpp
    src/main.cpp
    src/qjoystick.cpp
)

# another list, this time it includes all header files that should be treated with moc
SET(HEADERS
    include/MainWindow.h
    include/qjoystick.h
)

# some .ui files
SET(UI_FILES
    gui/mainwindow.ui
)

# enable warnings
ADD_DEFINITIONS( -Wall )

# this command finds Qt5 libraries and sets all required variables
FIND_PACKAGE(Qt5Widgets 
		REQUIRED )
		
FIND_PACKAGE(Qt5SerialPort 
		REQUIRED )		

FIND_PACKAGE(Qt5Network 
		REQUIRED )	

FIND_LIBRARY(SDL_LIB NAMES 
				SDL 
			 PATHS
			 /SDL/bin/
			 NO_DEFAULT_PATH
			 NO_SYSTEM_ENVIRONMENT_PATH
			 NO_CMAKE_PATH
			 NO_CMAKE_SYSTEM_PATH)
	
FIND_PATH(SDL_INCLUDE SDL/SDL.h /SDL/include)	

SET(TSA_COMPILE_FLAGS "${TSA_COMPILE_FLAGS} -DSDL_WIN")

# this will run uic on .ui files:
QT5_WRAP_UI(UI_HEADERS ${UI_FILES} )

# and finally this will run moc:
QT5_WRAP_CPP(MOC_HEADERS ${HEADERS} )

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES( 
			${CMAKE_BINARY_DIR}
			include/
			${SDL_INCLUDE}
                   )

# here we instruct CMake to build "sample" executable from all of the source files
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${MOC_HEADERS} ${UI_FILES} )

# last thing we have to do is to tell CMake what libraries our executable needs,
# luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Qt5::Widgets Qt5::SerialPort Qt5::Network ${SDL_LIB})


