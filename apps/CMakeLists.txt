add_subdirectory(yscenetrace)
add_subdirectory(yimgdenoise)

if(YOCTO_OPENGL)
add_subdirectory(ysceneitraces)
endif(YOCTO_OPENGL)
