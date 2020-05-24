add_executable(yimgdenoise yimgdenoise.cpp)

set_target_properties(yimgdenoise PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
target_include_directories(yimgdenoise PUBLIC ${CMAKE_SOURCE_DIR}/libs)
target_link_libraries(yimgdenoise yocto yocto_pathtrace)

find_library(OIDN_LIBRARIES NAMES OpenImageDenoise PATHS "${CMAKE_SOURCE_DIR}/apps/yimgdenoise/ext/oidn/lib")
target_include_directories(yimgdenoise PUBLIC ${CMAKE_SOURCE_DIR}/apps/yimgdenoise/ext/oidn/include)
target_link_libraries(yimgdenoise ${OIDN_LIBRARIES})
