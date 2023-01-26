project(QtAwesome)

add_library(${PROJECT_NAME}
  QtAwesome.cpp
  QtAwesomeAnim.cpp
  QtAwesome.qrc
)

target_include_directories(${PROJECT_NAME}
  INTERFACE ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Widgets
)
