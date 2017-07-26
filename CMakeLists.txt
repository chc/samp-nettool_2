cmake_minimum_required (VERSION 2.6)

project(nettool)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file (GLOB MAIN_SRCS "*.cpp")
file (GLOB MAIN_HDRS "*.h")

file (GLOB OS_PLAT_SRCS "OS/Threads/PThreads/*.cpp")
file (GLOB OS_PLAT_HDRS "OS/Threads/PThreads/*.h")

file (GLOB LEGACY_SRCS "legacy/*.cpp")
file (GLOB LEGACY_HDRS "legacy/*.h")
file (GLOB SAMP_SRCS "SAMP/*.cpp")
file (GLOB SAMP_HDRS "SAMP/*.h")
file (GLOB PYTHON_SRCS "SAMP/python/*.cpp")
file (GLOB PYTHON_HDRS "SAMP/python/*.h")
file (GLOB RAKNET_SRCS "SAMP/rak_minimal/*.cpp")
file (GLOB RAKNET_HDRS "SAMP/rak_minimal/*.h")

set (ALL_SRCS ${MAIN_SRCS} ${MAIN_HDRS} ${SAMP_SRCS} ${SAMP_HDRS} ${PYTHON_SRCS} ${PYTHON_HDRS} ${RAKNET_SRCS} ${RAKNET_HDRS} ${LEGACY_SRCS} ${LEGACY_HDRS} ${OS_PLAT_SRCS} ${OS_PLAT_HDRS})

include_directories ("." "D:\\Python36\\include" "SAMP/rak_minimal")

source_group("Sources" FILES ${MAIN_SRCS})
source_group("Sources\\OS" FILES ${OS_PLAT_SRCS})
source_group("Sources\\SAMP" FILES ${SAMP_SRCS})
source_group("Sources\\SAMP\\python" FILES ${PYTHON_SRCS})
source_group("Sources\\SAMP\\rak_minimal" FILES ${RAKNET_SRCS})
source_group("Sources\\Legacy" FILES ${LEGACY_SRCS})

source_group("Headers" FILES ${MAIN_HDRS})
source_group("Headers\\OS" FILES ${OS_PLAT_HDRS})
source_group("Headers\\SAMP" FILES ${SAMP_HDRS})
source_group("Headers\\SAMP\\python" FILES ${PYTHON_HDRS})
source_group("Headers\\SAMP\\rak_minimal" FILES ${RAKNET_HDRS})
source_group("Headers\\Legacy" FILES ${LEGACY_HDRS})

add_executable (nettool ${ALL_SRCS})
target_link_libraries(nettool D:\\Python36\\libs\\python36.lib)