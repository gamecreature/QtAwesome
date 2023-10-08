cmake_minimum_required(VERSION 3.16)
project(QtAwesome)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(QtAwesome
	QtAwesome/QtAwesome.h
	QtAwesome/QtAwesome.cpp
	QtAwesome/QtAwesomeAnim.h
	QtAwesome/QtAwesomeAnim.cpp
	QtAwesome/QtAwesomeEnumGenerated.h
	QtAwesome/QtAwesomeStringGenerated.h
	QtAwesome/QtAwesomeFree.qrc
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

install(TARGETS QtAwesome EXPORT QtAwesomeConfig
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINARYDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT QtAwesomeConfig
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtAwesome/
	NAMESPACE QtAwesome::
)
export(EXPORT QtAwesomeConfig NAMESPACE QtAwesome::)

install(FILES
  QtAwesome/QtAwesome.h
  QtAwesome/QtAwesomeAnim.h
  QtAwesome/QtAwesomeEnumGenerated.h
  QtAwesome/QtAwesomeStringGenerated.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QtAwesome
  )

