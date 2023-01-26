project(QtAwesome)

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

include(GenerateExportHeader)
generate_export_header(QtAwesome
  BASE_NAME
  EXPORT_MACRO_NAME QTAWESOME_EXPORT
  EXPORT_FILE_NAME QtAwesome_export.h
  STATIC_DEFINE QTAWESOME_BUILT_AS_STATIC)
