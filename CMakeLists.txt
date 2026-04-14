cmake_minimum_required(VERSION 3.10)
project(MinecraftServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DSTANDALONE_SERVER)
add_definitions(-DLINUX)

# Include directories
include_directories(src)
include_directories(libraries/include)

# Source files (Minimal set for server)
set(SOURCES
    src/main_dedicated.cpp
    src/NinecraftApp.cpp
    src/Performance.cpp
    src/SharedConstants.cpp
    src/client/Minecraft.cpp
    src/client/Options.cpp
    src/client/OptionsFile.cpp
    src/client/OptionStrings.cpp
    src/client/IConfigListener.cpp
    src/locale/I18n.cpp
    src/nbt/Tag.cpp
    src/network/ClientSideNetworkHandler.cpp
    src/network/NATPunchHandler.cpp
    src/network/NetEventCallback.cpp
    src/network/Packet.cpp
    src/network/PHPDirectoryServer2.cpp
    src/network/RakNetInstance.cpp
    src/network/ServerSideNetworkHandler.cpp
    src/network/command/CommandServer.cpp
    src/platform/CThread.cpp
    src/platform/time.cpp
    src/platform/input/Controller.cpp
    src/platform/input/Keyboard.cpp
    src/platform/input/Mouse.cpp
    src/platform/input/Multitouch.cpp
    src/util/DataIO.cpp
    src/util/Mth.cpp
    src/util/PerfTimer.cpp
    src/util/StringUtils.cpp
)

# Add all server files
file(GLOB_RECURSE SERVER_FILES "src/server/*.cpp")
list(APPEND SOURCES ${SERVER_FILES})

# Add all world files
file(GLOB_RECURSE WORLD_FILES "src/world/*.cpp")
list(APPEND SOURCES ${WORLD_FILES})

# Add all RakNet files
file(GLOB_RECURSE RAKNET_FILES "src/raknet/*.cpp")
list(APPEND SOURCES ${RAKNET_FILES})

# Remove some specifically platform-dependent or unnecessary files if any
list(REMOVE_ITEM SOURCES src/raknet/VitaIncludes.cpp)
list(REMOVE_ITEM SOURCES src/raknet/Rackspace.cpp)

# Dependencies
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

add_executable(MinecraftServer ${SOURCES})
target_link_libraries(MinecraftServer ZLIB::ZLIB ${CMAKE_THREAD_LIBS_INIT} dl)

# Build settings
if(MSVC)
    target_compile_options(MinecraftServer PRIVATE /utf-8)
else()
    target_compile_options(MinecraftServer PRIVATE -Wno-write-strings -fpermissive)
endif()
