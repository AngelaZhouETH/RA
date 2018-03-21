set(CMAKE_BUILD_TYPE Debug)

link_libraries(D3D)
INCLUDE_DIRECTORIES(MultiClassGrid)

add_executable(readAndConvert16bitDM readAndConvert16bitDM.cpp)

target_link_libraries(readAndConvert16bitDM
 ${Boost_PROGRAM_OPTIONS_LIBRARY}
 ${Boost_FILESYSTEM_LIBRARY}
 ${Boost_ALGORITHM_LIBRARY}
)

set_source_files_properties(readAndConvert16bitDM.cpp PROPERTIES COMPILE_FLAGS "-std=c++11")

add_executable(readAndConvertSUNCGCategories readAndConvertSUNCGCategories.cpp)

target_link_libraries(readAndConvertSUNCGCategories
 ${Boost_PROGRAM_OPTIONS_LIBRARY}
 ${Boost_FILESYSTEM_LIBRARY}
 ${Boost_ALGORITHM_LIBRARY}
)

set_source_files_properties(readAndConvertSUNCGCategories.cpp PROPERTIES COMPILE_FLAGS "-std=c++11")

