project(QtAwesome)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(${PROJECT_NAME}
  QtAwesome.cpp
  QtAwesomeAnim.cpp
  QtAwesomeFree.qrc
)

target_include_directories(${PROJECT_NAME}
  INTERFACE ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Widgets
)
