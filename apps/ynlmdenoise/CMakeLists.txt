add_executable(ynlmdenoise ynlmdenoise.cpp)

set_target_properties(ynlmdenoise PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
target_include_directories(ynlmdenoise PUBLIC ${CMAKE_SOURCE_DIR}/libs)
target_link_libraries(ynlmdenoise yocto yocto_extension)


