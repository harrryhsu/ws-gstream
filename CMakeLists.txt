project (stream)
cmake_minimum_required(VERSION 3.16)

find_package(PkgConfig REQUIRED)
find_package(libwebsockets CONFIG REQUIRED)

pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0>=1.4)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.4)
pkg_search_module(websocket REQUIRED IMPORTED_TARGET libwebsockets)

include_directories(./src)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(stream src/main.cpp)

target_link_libraries(stream
    PkgConfig::gstreamer
    PkgConfig::gstreamer-sdp
    PkgConfig::gstreamer-app
    PkgConfig::gstreamer-video
		Threads::Threads
		websockets_shared
		${LIBWEBSOCKETS_DEP_LIBS}
)