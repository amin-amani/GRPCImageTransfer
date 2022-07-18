#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>


#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include "PbfImage.grpc.pb.h"
#include <QDebug>
using namespace std;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
typedef unsigned char byte;



// Reverse conversion from protobuf to cv::Mat
cv::Mat get_cvImage(const PbfImage &img_msg) {
    char* img;
    img=new  char [img_msg.height()*img_msg.width()*3];
    std::memcpy(img, img_msg.data().c_str(), img_msg.height()*img_msg.width()*3);
    return cv::Mat(img_msg.height(), img_msg.width(),CV_8UC3,&img[0]);
}

// Forward conversion from cv::Mat to protobuf
PbfImage get_PbfImage(const cv::Mat &cv_img) {
unsigned char* img_msg ;
//    std::unique_ptr<unsigned char> img_msg (new unsigned char [cv_img.total()*3]);
    img_msg =new  unsigned char [cv_img.cols*cv_img.rows* 3];

    PbfImage result;
   std::memcpy(img_msg, cv_img.data,cv_img.cols*cv_img.rows* 3 );
    result.set_data((char*)img_msg,cv_img.cols*cv_img.rows* 3);
    result.set_color(true);
    result.set_width(cv_img.cols);
    result.set_height(cv_img.rows);
    return result;
}

class ImageServiceImpl final : public PbfImageService::Service {


public:
        Status SendImage(ServerContext* context, const PbfImage* request, PbfImage* reply) {


            qDebug()<<"service called";
            cv::Mat image=get_cvImage(*request);
            cv::imshow("image",image);
            cv::waitKey(0);
            return Status::OK;
        }
};





void RunServer() {

    printf("Runserver function\n");

    std::string server_address("0.0.0.0:50051");
    ImageServiceImpl service;

    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();

}


int main(int argc, char ** argv) {
    RunServer();
    return 0;
}

