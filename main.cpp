#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
    VideoCapture source("5_lt_cw_100rpm_out.mp4");
    if (!source.isOpened()){ cout << "Video file error" << endl; return -1; }

    string dst = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=203.234.58.168 port=8001 sync=false";

    VideoWriter writer(dst, 0, (double)30, Size(640,360), true);
    if(!writer.isOpened()) {cerr<<"Writer open failed!"<<endl; return -1;}

    string dst1 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=203.234.58.168 port=8002 sync=false";

    VideoWriter writer1(dst1, 0, (double)30, Size(640,90), false);
    if(!writer1.isOpened()) {cerr<<"Writer open failed!"<<endl; return -1;}

    Mat frame, gray, binary, frame1;
    const int TARGET_MEAN = 100; // 목표 평균 밝기

    while (true) {
        source >> frame;
        if (frame.empty()){ cerr << "frame empty!" << endl; break; }

        Rect crop1(0, 270, 640, 90);
        frame1 = frame(crop1);

        cvtColor(frame1, gray, COLOR_BGR2GRAY);

        // 밝기 보정
        Scalar meanBrightness = mean(gray);
        gray = gray + (TARGET_MEAN - meanBrightness[0]);

        
        //adaptiveThreshold(gray, binary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, 2);// 적응형 이진화

        threshold(gray, binary, 128, 255, THRESH_BINARY);  // 이진화 처리

        writer << frame;  // 원본 영상 스트리밍
        writer1 << binary; // 이진화 영상 스트리밍

        waitKey(30);
    }
    return 0;
}








