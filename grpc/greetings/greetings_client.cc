
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <string.h>
#include "PbfImage.grpc.pb.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"



typedef unsigned char byte;

using std::cout;
using std::cin;
using std::endl;


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

class ImageClientImpl {
private:
    std::unique_ptr<PbfImageService::Stub> stub_;
public:
    ImageClientImpl(std::shared_ptr<grpc::Channel> channel) : stub_(PbfImageService::NewStub(channel)) {}

    void SendImage(char* fileName)
    {

        grpc::ClientContext context;
        PbfImage request;// = new PbfImage();
        cv::Mat frame=cv::imread(fileName);

        PbfImage reply;
       request= get_PbfImage(frame);
        grpc::Status status = stub_->SendImage(&context, request, &reply);
    }

};

int main() {



    ImageClientImpl imageChannel(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    imageChannel.SendImage("/home/amin/GRPC/gRPC-Examples/grpc/greetings/1.png");
    return 0;

}


