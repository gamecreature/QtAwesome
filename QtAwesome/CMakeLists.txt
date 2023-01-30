project(QtAwesome)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(QtAwesome
  QtAwesome.cpp
  QtAwesomeAnim.cpp
  QtAwesomeFree.qrc
)

target_include_directories(QtAwesome
  INTERFACE ${PROJECT_SOURCE_DIR}
)

target_link_libraries(QtAwesome PUBLIC
  Qt${QT_VERSION_MAJOR}::Widgets
)
