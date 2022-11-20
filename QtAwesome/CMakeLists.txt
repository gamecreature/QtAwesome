project(QtAwesome)

add_library(QtAwesome
  QtAwesome.cpp
  QtAwesomeAnim.cpp
  QtAwesome.qrc
)

target_include_directories(QtAwesome
  PUBLIC ${PROJECT_SOURCE_DIR}
)

target_link_libraries(QtAwesome PUBLIC
  Qt5::Widgets
)
