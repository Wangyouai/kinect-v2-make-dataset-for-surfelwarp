#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include<algorithm>
#include<fstream>
#include <string>
#include <cstdlib>
using namespace std;
using namespace cv;

enum Processor { cl, gl, cpu };

int counters = 0;
string save_path = "/home/wya/project/kinect/Dataset";  //根据自己需要修改
nt main(int argc, char *argv[])
{
    //! [context]
    libfreenect2::Freenect2 freenect2;
    libfreenect2::Freenect2Device *dev = nullptr;
    libfreenect2::PacketPipeline *pipeline = nullptr;
    //! [context]
    //! [discovery]
    if(freenect2.enumerateDevices() == 0)
    {
        std::cout << "no device connected!" << std::endl;
        return -1;
    }
    string serial = freenect2.getDefaultDeviceSerialNumber();
    if(serial == "")  return -1;
    cout<<"The serial number is :"<<serial<<endl;
    //! [discovery]

    int depthProcessor = Processor::cl;
    if(depthProcessor == Processor::cpu)
    {
        if(!pipeline)
        {
            //! [pipeline]
            pipeline = new libfreenect2::CpuPacketPipeline();
            //! [pipeline]
        }
    }
    else
    if (depthProcessor == Processor::gl)
    {
#ifdef LIBFREENECT2_WITH_OPENGL_SUPPORT
        if(!pipeline)
                                pipeline = new libfreenect2::OpenGLPacketPipeline();
#else
        std::cout << "OpenGL pipeline is not supported!" << std::endl;
#endif
    }
    else
    if (depthProcessor == Processor::cl)
    {
#ifdef LIBFREENECT2_WITH_OPENCL_SUPPORT
        if(!pipeline)
                                        pipeline = new libfreenect2::OpenCLPacketPipeline();
#else
        std::cout << "OpenCL pipeline is not supported!" << std::endl;
#endif
    }    if(pipeline)
    {
        //! [open]
        dev = freenect2.openDevice(serial, pipeline);
        //! [open]
    }
    else
    {
        dev = freenect2.openDevice(serial);
    }
    if(dev == 0)
    {
        std::cout << "failure opening device!" << std::endl;
        return -1;
    }

    //! [listeners]
    libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color |libfreenect2::Frame::Depth |libfreenect2::Frame::Ir);
    libfreenect2::FrameMap frames;
    dev->setColorFrameListener(&listener);
    dev->setIrAndDepthFrameListener(&listener);
    //! [listeners]

    //! [start]
    dev->start();
    std::cout << "device serial: " << dev->getSerialNumber() << std::endl;
    std::cout << "device firmware: " << dev->getFirmwareVersion() << std::endl;
    //! [start]
    libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
    libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4);
    //Mat rgbmat, depthmat, irmat, depthmatUndistorted, rgbd, rgbd2;

    string imagergb = "image";
    string imaged = "depth";
    string lie;


    while(!(waitKey(1)==27))
    {
        listener.waitForNewFrame(frames);
        libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
        // libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
        libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];
        //! [loop start]

        //! [registration] depth2RGB
        registration->apply(rgb, depth, &undistorted, &registered);

        cv::Size dsize = cv::Size(512, 424);
        Mat rgbmat = cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data);
        Mat depthmat =cv::Mat(depth->height, depth->width, CV_32FC1, depth->data);
        Mat depth2rgb =cv::Mat(registered.height, registered.width, CV_8UC4, registered.data);

        rgbmat.convertTo(rgbmat,CV_8UC3,1,0);
        depthmat.convertTo(depthmat,CV_16UC1,1,0);

        cv::resize(rgbmat,rgbmat,dsize);
        cv::resize(depthmat,depthmat,dsize);

        cv::imshow("rgb", rgbmat);
        cv::imshow("depth", depthmat);
        cv::imshow("depth2rgb", depth2rgb);
        cv::waitKey(1);

        string picname_rgb =to_string(counters);
        string picname_depth =to_string(counters);
        if(counters<10){
            picname_rgb = save_path + "/rgb/" +"frame-00000"+picname_rgb + ".color.png";
            picname_depth = save_path + "/depth/" +"frame-00000"+picname_depth + ".depth.png";
        }

        else if(counters>=10 && counters<100){
            picname_rgb = save_path + "/rgb/" +"frame-0000"+picname_rgb + ".color.png";
            picname_depth = save_path + "/depth/" +"frame-0000"+picname_depth + ".depth.png";
        }
        else if(counters>=100 && counters<1000){
            picname_rgb = save_path + "/rgb/" +"frame-000"+picname_rgb + ".color.png";
            picname_depth = save_path + "/depth/" +"frame-000"+picname_depth + ".depth.png";
        }

        else if(counters>=1000 && counters<10000){
            picname_rgb = save_path + "/rgb/" +"frame-00"+picname_rgb + ".color.png";
            picname_depth = save_path + "/depth/" +"frame-00"+picname_depth + ".depth.png";
        }

        else if(counters>=10000 && counters<100000){
            picname_rgb = save_path + "/rgb/" +"frame-0"+picname_rgb + ".color.png";
            picname_depth = save_path + "/depth/" +"frame-0"+picname_depth + ".depth.png";
        }

        else{
            picname_rgb = save_path + "/rgb/" +"frame-"+picname_rgb + ".color.png";
            picname_depth = save_path + "/depth/" +"frame-"+picname_depth + ".depth.png";
        }
        imwrite(picname_rgb, rgbmat);//保存rgb图片
        imwrite(picname_depth, depthmat);//保存deoth图片
        std::cout << "输出第  " << counters << "  幅图像" << std::endl;

        // cv::Mat(undistorted.height, undistorted.width, CV_32FC1, undistorted.data).copytTo(depthmatUndistorted);
        // cv::Mat(registered.height, registered.width, CV_8UC4, registered.data).copyTo(rgbd);
        // cv::Mat(depth2rgb.height, depth2rgb.width, CV_32FC1, depth2rgb.data).copyTo(rgbd2);


        // cv::imshow("undistorted", depthmatUndistorted / 4500.0f);
        // cv::imshow("registered", rgbd);
        // cv::imshow("depth2RGB", rgbd2 / 4500.0f);
        //! [loop end]
        listener.release(frames);
        counters++;
    }
    //! [loop end]

    //! [stop]
    dev->stop();
    dev->close();
    //! [stop]

    delete registration;

    std::cout << "Goodbye World!" << std::endl;
    return 0;
}
