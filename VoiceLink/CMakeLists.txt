cmake_minimum_required(VERSION 3.16)
project("VoiceLink")
set(CMAKE_CXX_STANDARD 17)

set(VL_SOURCES src/main.cc)

add_library(VoiceLink SHARED ${VL_SOURCES})
target_include_directories(VoiceLink PRIVATE third-party/SimConnect/include)
if(CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(VoiceLink ${CMAKE_SOURCE_DIR}/third-party/SimConnect/lib/static/SimConnect_debug.lib)
else()
target_link_libraries(VoiceLink ${CMAKE_SOURCE_DIR}/third-party/SimConnect/lib/static/SimConnect.lib)
endif()
target_link_libraries(VoiceLink ws2_32 shlwapi ole32)
