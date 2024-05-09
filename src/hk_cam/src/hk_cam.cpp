#include <iostream>
#include <numeric>
#include <signal.h>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/logger.hpp>
#include <unistd.h>
// #include <cv_bridge/cv_bridge.h>
#include "std_msgs/msg/header.hpp"

#include <sensor_msgs/msg/image.hpp>
// #include "hk_interfaces/"
#include <opencv2/opencv.hpp>


#include "hk_interfaces/srv/hk_cam_srv.hpp"


#include "HCNetSDK.h"
#include "LinuxPlayM4.h"
#include "iniFile.h"

bool need_exit = true;

typedef unsigned char *PBYTE;
LONG nport = 1;




class HK_Node : public rclcpp::Node
{
public:


void handle_move_robot(const std::shared_ptr<hk_interfaces::srv::HkCamSrv::Request> request,
                         std::shared_ptr<hk_interfaces::srv::HkCamSrv::Response> response)
{

}

    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_img;
//    rclcpp::Service<>::SharedPtr ptz_ser;
    HK_Node() : Node("hk_node")
    {
        RCLCPP_INFO(this->get_logger(), "lidar_node init ...");
        pub_img = this->create_publisher<sensor_msgs::msg::Image>("hk_img",
                                                                  10);
        this->create_service<hk_interfaces::srv::HkCamSrv>("test",
            std::bind(&HK_Node::handle_move_robot,
                                this,
                                std::placeholders::_1,
                                std::placeholders::_2));



        // HK_Node::hk_show2();
    }
    ~HK_Node() override
    {
        std::cout << "-----" << std::endl;
    }

private:
    int hk_show();
    

public:
    int hk_show2();
    int GetStream();
    // void PsDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pPacketBuffer, DWORD nPacketSize, void *pUser);
};

void PsDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pPacketBuffer, DWORD nPacketSize, void *pUser)
{
    printf("--回调");
    if (dwDataType == NET_DVR_SYSHEAD)
    {
        PlayM4_SetStreamOpenMode(nport, 0);
        if (PlayM4_OpenStream(nport, pPacketBuffer, nPacketSize, 1024 * 1024))
            if (PlayM4_Play(nport, NULL))
                std::cout << "播放库播放成功" << std::endl;
            else
                std::cout << "播放库播放失败" << std::endl;
        else
            std::cout << "播放库打开流失败" << std::endl;
    }
    else if (dwDataType == NET_DVR_STREAMDATA)
        PlayM4_InputData(nport, pPacketBuffer, nPacketSize);
}



int HK_Node::GetStream()
{
    // 从配置文件读取设备信息
    //    IniFile ini("Device.ini");
    unsigned int dwSize = 0;
    //    char sSection[16] = "DEVICE";
    //
    int iRealPlayHandle = 0;
    //    char *sIP = ini.readstring(sSection, "ip", "error", dwSize);
    //    int iPort = ini.readinteger(sSection, "port", 0);
    //    char *sUserName = ini.readstring(sSection, "username", "error", dwSize);
    //    char *sPassword = ini.readstring(sSection, "password", "error", dwSize);
    //    int iChannel = ini.readinteger(sSection, "channel", 1);
    //    char *sIP = "192.168.44.64";
    //    char *sUserName = "admin";
    //    char *sPassword = "air12345678";
    NET_DVR_DEVICEINFO_V30 struDeviceInfo;
    // int iUserID = NET_DVR_Login_V30(sIP, iPort, sUserName, sPassword, &struDeviceInfo);
    int iUserID = NET_DVR_Login_V30("192.168.44.64", 8011, "admin", "air12345678", &struDeviceInfo);
    if (iUserID >= 0)
    {

        // NET_DVR_CLIENTINFO ClientInfo = {0};
        // ClientInfo.lChannel     = iChannel;  //channel NO.
        // ClientInfo.lLinkMode    = 0;
        // ClientInfo.sMultiCastIP = NULL;
        // int iRealPlayHandle = NET_DVR_RealPlay_V30(iUserID, &ClientInfo, PsDataCallBack, NULL, 0);
        NET_DVR_PREVIEWINFO struPreviewInfo = {0};
        struPreviewInfo.lChannel = 1;
        struPreviewInfo.dwStreamType = 1;
        struPreviewInfo.dwLinkMode = 1;
        struPreviewInfo.bBlocked = 1;
        struPreviewInfo.hPlayWnd = NULL;
        struPreviewInfo.bPassbackRecord = 0;
        iRealPlayHandle = NET_DVR_RealPlay_V40(iUserID, &struPreviewInfo, PsDataCallBack, NULL);
        if (iRealPlayHandle >= 0)
        {
            printf("[GetStream]---RealPlay %d success, \n", 1, NET_DVR_GetLastError());
            // int iRet = NET_DVR_SaveRealData(iRealPlayHandle, "./record/realplay.dat");
            // NET_DVR_SetStandardDataCallBack(iRealPlayHandle, StandardDataCallBack, 0);
        }
        else
        {
            printf("[GetStream]---RealPlay %d failed, error = %d\n", 1, NET_DVR_GetLastError());
        }
    }
    else
    {
        printf("[GetStream]---Login failed, error = %d\n", NET_DVR_GetLastError());
        need_exit = false;
        NET_DVR_Cleanup();
        rclcpp::shutdown();
        exit;
    }
    // return iRealPlayHandle;
    return iUserID;
}


int HK_Node::hk_show2()
{
    sleep(1);
    DWORD dwPicSize = 1024 * 1024 * 3;
    PBYTE pJpeg2 = NULL;
    DWORD size = 0;
    BOOL ret = 0;
    std::cout << "nport = " << nport << std::endl;
    //    for (int i = 0; i < 5000; i++)
    while (need_exit)
    {
        /* code */
        pJpeg2 = new unsigned char[dwPicSize];

        ret = PlayM4_GetJPEG(nport, pJpeg2, dwPicSize, &size);

        cv::Mat frame(1, size, CV_8UC1, pJpeg2);
        std::cout << ret << std::endl;
        cv::Mat image = cv::imdecode(frame, cv::IMREAD_COLOR);

        //        cv::Mat grayImg;

        // 将彩色图像转换为灰度图像
        //        cv::cvtColor(image, grayImg, cv::COLOR_BGR2GRAY);

        cv::Size size = image.size();
        std::cout << "Width: " << size.width << ", Height: " << size.height << std::endl;

        // 定义剪裁区域，这里我们剪裁图片的中心部分
        //        int crop_x = size.width / 4;
        //        int crop_y = size.height / 4;
        //        int crop_width = size.width / 2;
        //        int crop_height = size.height / 2;
        //        cv::Rect roi(0, 0, crop_width, crop_height);

        // 剪裁图片
        cv::Mat cropped_img = image;
        // cv::Mat color_image(1080, 1920, CV_8UC3);
        auto msg = sensor_msgs::msg::Image();
        cv::Mat frame2;
        msg.header.stamp = this->now();
        msg.header.frame_id = "2";

        frame2 = cropped_img;

        msg.width = frame2.cols;
        msg.height = frame2.rows;
        msg.encoding = "bgr8";
        msg.is_bigendian = false;
        msg.step = frame2.cols * frame2.rows;
        sensor_msgs::msg::Image::_data_type data_msg;
        data_msg.resize(frame2.total() * frame2.elemSize());
        std::memcpy(
            data_msg.data(), frame2.data,
            frame2.total() * frame2.elemSize());
        msg.data = data_msg;
        std::cout << msg.width << std::endl;
        pub_img->publish(msg);

        char name[] = "name-%d-";
        // 检查图像是否正确解码
        if (!image.empty())
        {

            //            printf("----%d----\n",i);
            //            sprintf(name,"name-%d-",i);
            cv::imshow("Decoded JPEG Image", cropped_img);
            // 等待按键事件，0表示无限等待
            cv::waitKey(1);
        }
        else
        {
            std::cerr << "无法解码JPEG图像数据" << std::endl;
        }

        delete[] pJpeg2;
    }
}

int HK_Node::hk_show()
{
    auto msg = sensor_msgs::msg::Image();

    msg.header.stamp = this->now();
    msg.header.frame_id = "1";
    cv::VideoCapture cap;
    cv::Mat frame;
    int deviceID = 0;
    int apiID = cv::CAP_ANY;
    cap.open(deviceID + apiID);
    if (!cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera" << std::endl;
        return -1;
    }
    while (need_exit)
    {
        // 循环读取图片;
        if (!cap.read(frame))
        {
            std::cout << "No frame" << std::endl;
            cv::waitKey(0);
        }
        msg.width = frame.cols;
        msg.height = frame.rows;
        msg.encoding = "bgr8";
        msg.is_bigendian = false;
        msg.step = frame.cols * 3;
        sensor_msgs::msg::Image::_data_type data_msg;
        data_msg.resize(frame.total() * frame.elemSize());
        std::memcpy(
            data_msg.data(), frame.data,
            frame.total() * frame.elemSize());
        msg.data = data_msg;

        pub_img->publish(msg);
        //        std::this_thread::sleep_for(std::chrono::seconds(1));
        // 显示图片;
        cv::imshow("Output Window", frame);

        if (cv::waitKey(1) >= 0)
        {
            cv::destroyAllWindows();
            break;
        };
    }
}

void ExitHandler(int sig)
{
    (void)sig;
    need_exit = false;
    std::cout << "--------------------------shutdown" << std::endl;
    cv::destroyAllWindows();
    NET_DVR_Cleanup();
    rclcpp::shutdown();
}

int main(int argc, char *argv[])
{
    NET_DVR_Init();
    std::cout << "PlayM4_GetPort(&nport); = " << PlayM4_GetPort(&nport) << std::endl;

    //    NET_DVR_SetLogToFile(3, "./record/");
    auto node = std::make_shared<HK_Node>();
    int iUserID = node->GetStream();
    // int iUserID = GetStream();
    std::cout << "iUserID=" << iUserID << "\r"
              << NET_DVR_GetLastError() << std::endl;
    if (iUserID>=0)
    {

        rclcpp::init(argc, argv);
        signal(SIGINT, ExitHandler);
        // auto node = std::make_shared<HK_Node>();
        rclcpp::spin(node);
    }
    return 0;
}
