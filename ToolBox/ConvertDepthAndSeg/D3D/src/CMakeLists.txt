include_directories(/opt/X11/include)


set(D3D_BASE_SRC_FILES
  d3d_base/exception.h
  d3d_base/grid.cpp
  d3d_base/grid.h
  d3d_base/configFile.cpp
  d3d_base/cameraMatrix.cpp
  d3d_base/depthMap.cpp
  d3d_base/colorMapJet.cpp
)

set(D3D_IO_SRC_FILES
  d3d_io/ioTools.cpp
)

add_library(D3D SHARED
  ${D3D_BASE_SRC_FILES}
  ${D3D_IO_SRC_FILES}
)

target_link_libraries(D3D
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${OpenCV_LIBRARIES}
  /opt/X11/lib
)

