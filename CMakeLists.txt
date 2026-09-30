cmake_minimum_required(VERSION 4.0)
project(TELESim)

set(CMAKE_CXX_STANDARD 20)

add_library(TELESimCore OBJECT
        devices/Device.cpp
        devices/Device.h
        devices/Radio.cpp
        devices/Radio.h
        devices/Phone.cpp
        devices/Phone.h
        devices/info/DeviceId.cpp
        devices/info/DeviceId.h
        devices/info/IpAddress.cpp
        devices/info/IpAddress.h
        devices/MasterRadio.cpp
        devices/MasterRadio.h
        utils/Logger.cpp
        utils/Logger.h
        utils/UtilsFunctions.cpp
        utils/UtilsFunctions.h
        fwk/DeviceWindowFactory.cpp
        fwk/DeviceWindowFactory.h
        fwk/IpAddressMaster.cpp
        fwk/IpAddressMaster.h
        fwk/DeviceFactory.cpp
        fwk/DeviceFactory.h
        fwk/UIHandler.cpp
        fwk/UIHandler.h
        HTTP_view/emit/PhoneEmitter.cpp
        HTTP_view/emit/PhoneEmitter.h
        HTTP_view/emit/RadioEmitter.cpp
        HTTP_view/emit/RadioEmitter.h
        HTTP_view/emit/MasterEmitter.cpp
        HTTP_view/emit/MasterEmitter.h
        HTTP_view/listen/PhoneListener.cpp
        HTTP_view/listen/PhoneListener.h
        HTTP_view/listen/RadioListener.cpp
        HTTP_view/listen/RadioListener.h
        HTTP_view/listen/MasterListener.cpp
        HTTP_view/listen/MasterListener.h
        HTTP_view/APIEndPoint.h
        devices/info/CallId.h
        devices/Packet.h)

target_compile_options(TELESimCore PRIVATE -g0)

add_executable(TELESim
        main.cpp
        $<TARGET_OBJECTS:TELESimCore>)

target_link_libraries(TELESim PRIVATE ws2_32)

add_executable(DeviceWindow
        DeviceWindow.cpp
        utils/DeviceInfoFileParser.cpp
        utils/DeviceInfoFileParser.h
        utils/ERole.h
        $<TARGET_OBJECTS:TELESimCore>)

target_link_libraries(DeviceWindow PRIVATE ws2_32)
target_compile_options(TELESim PRIVATE -g0)
target_compile_options(DeviceWindow PRIVATE -g0)

set(REG_PATH "${CMAKE_SOURCE_DIR}/reg.txt")

add_compile_definitions(
        PROJECT_PATH="${CMAKE_SOURCE_DIR}"
        REG_PATH="${REG_PATH}"
        MASTER_PORT=8081
        _WIN32_WINNT=0x0A00
)