cmake_minimum_required(VERSION 3.24)
project(TcpServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH /home/fumoumou/Qt5.14.2/5.14.2/gcc_64)
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Sql
        REQUIRED)

add_executable(TcpServer main.cpp UI/tcpserver.cpp func/mytcpserver.cpp func/mytcpserver.h func/mytcpsocket.cpp func/mytcpsocket.h func/portocol.cpp func/OpeDB.cpp func/OpeDB.h)
target_link_libraries(TcpServer
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        Qt5::Sql
        )

