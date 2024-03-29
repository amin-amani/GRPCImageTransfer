# Minimum CMake required
cmake_minimum_required(VERSION 3.13)

project(gRPC_CPP_Greetings VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set (proto_name PbfImage)
set (binary_name greetings )
#set (proto_name greetings )
# Get proto files
get_filename_component(proto "../../protos/${proto_name}.proto" ABSOLUTE)
get_filename_component(proto_dir "${proto}" DIRECTORY)

# Generate source files
set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.h")
add_custom_command(
    OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${proto_dir}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${proto}"
    DEPENDS "${proto}"
)

set(targets "${binary_name}_server" "${binary_name}_client")
set(OPENCV_INC "/usr/local/include/opencv4/")
# Include generated *.pb.h files
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${OPENCV_INC})

set(OPENCV_LIBS
    "/usr/local/lib/libopencv_core.so"
    "/usr/local/lib/libopencv_highgui.so"
    "/usr/local/lib/libopencv_imgproc.so"
    "/usr/local/lib/libopencv_imgcodecs.so"
    "/usr/local/lib/libopencv_videoio.so"
    "/usr/local/lib/libopencv_video.so"
    "/usr/local/lib/libopencv_dnn.so"
        )




foreach(target ${targets})
    add_executable(${target} "${target}.cc" ${proto_srcs} ${grpc_srcs})
    target_link_libraries(${target} PRIVATE ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF}  ${OPENCV_LIBS} Qt5::Core
        Qt5::Gui
        Qt5::Widgets)
    # Cannot do this because these directories are not available during CMake "compile time"
    # target_include_directories(${proto_hdrs} ${grpc_hdrs})
endforeach()

