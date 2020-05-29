add_subdirectory(yscenetrace)
add_subdirectory(yimgdenoise)
add_subdirectory(ynlmdenoise)

if(YOCTO_OPENGL)
add_subdirectory(ysceneitraces)
endif(YOCTO_OPENGL)
