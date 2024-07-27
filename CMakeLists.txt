cmake_minimum_required(VERSION 3.16)

#Set The Current Version Number to use as fallback if GIT Fails.
set(QTAWESOME_VERSION_MAJOR 6)
set(QTAWESOME_VERSION_MINOR 5)
set(QTAWESOME_VERSION_PATCH 1)
set(QTAWESOME_VERSION_TWEAK 0)
# Get the version from git if it's a git repository
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  find_package(Git)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} describe --tags --long --match "font-awesome-*" --always
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE GITREV
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
      string(FIND ${GITREV} "font-awesome-" isRev)
      if(NOT ifRev EQUAL -1)
        string(REGEX MATCH [0-9]+ MAJOR ${GITREV})
        string(REGEX MATCH \\.[0-9]+ MINOR ${GITREV})
        string(REPLACE "." "" MINOR "${MINOR}")
        string(REGEX MATCH [0-9]+\- PATCH ${GITREV})
        string(REPLACE "-" "" PATCH "${PATCH}")
        string(REGEX MATCH \-[0-9]+\- TWEAK ${GITREV})
        string(REPLACE "-" "" TWEAK "${TWEAK}")
        set(QTAWESOME_VERSION_MAJOR ${MAJOR})
        set(QTAWESOME_VERSION_MINOR ${MINOR})
        set(QTAWESOME_VERSION_PATCH ${PATCH})
        set(QTAWESOME_VERSION_TWEAK ${TWEAK})
      elseif(NOT ${GITREV} STREQUAL "")
        set(QTAWESOME_VERSION_TWEAK ${GITREV})
      endif()
  endif()
endif()

set(QTAWESOME_VERSION "${QTAWESOME_VERSION_MAJOR}.${QTAWESOME_VERSION_MINOR}.${QTAWESOME_VERSION_PATCH}.${QTAWESOME_VERSION_TWEAK}")
message(STATUS "Building QTAWESOME: ${QTAWESOME_VERSION}")

project(QtAwesome VERSION ${QTAWESOME_VERSION})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(QtAwesome_HEADERS
	QtAwesome/QtAwesome.h
	QtAwesome/QtAwesomeAnim.h
	QtAwesome/QtAwesomeEnumGenerated.h
	QtAwesome/QtAwesomeStringGenerated.h
)

add_library(QtAwesome
	QtAwesome/QtAwesome.cpp
	QtAwesome/QtAwesomeAnim.cpp
	QtAwesome/QtAwesomeFree.qrc
	${QtAwesome_HEADERS}
)

include(GNUInstallDirs)

target_include_directories(QtAwesome
	INTERFACE
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/QtAwesome>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(QtAwesome PUBLIC
	Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(QtAwesome PROPERTIES
	PUBLIC_HEADER "${QtAwesome_HEADERS}"
)

install(TARGETS QtAwesome EXPORT QtAwesomeConfig
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QtAwesome
)

install (FILES "${PROJECT_SOURCE_DIR}/LICENSE.md" DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/${PROJECT_NAME} RENAME LICENSE)

install(EXPORT QtAwesomeConfig
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtAwesome/
	NAMESPACE QtAwesome::
)

export(EXPORT QtAwesomeConfig NAMESPACE QtAwesome::)
