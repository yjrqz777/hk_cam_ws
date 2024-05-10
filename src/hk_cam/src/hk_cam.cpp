#include <iostream>
//#include <numeric>
#include <signal.h>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/logger.hpp>

// #include <cv_bridge/cv_bridge.h>
//#include "std_msgs/msg/header.hpp"

//#include <sensor_msgs/msg/image.hpp>
// #include "hk_interfaces/"
//#include <opencv2/opencv.hpp>

//#include "hk_interfaces/srv/hk_cam_srv.hpp"

//#include "HCNetSDK.h"
//#include "LinuxPlayM4.h"
//#include "iniFile.h"

#include "hk_cam/hk_class.h"


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
    rclcpp::init(argc, argv);
    //    NET_DVR_SetLogToFile(3, "./record/");
    auto node = std::make_shared<HK_Node>("hk_node");
    int iUserID = node->GetStream();
//     int iUserID = 0;
    std::cout << "iUserID=" << iUserID << "\r"
              << NET_DVR_GetLastError() << std::endl;
    if (iUserID >= 0)
    {
        signal(SIGINT, ExitHandler);
        rclcpp::executors::MultiThreadedExecutor executor;
        executor.add_node(node);
        executor.spin();
//        executor.
//        rclcpp::spin(node);
    }
    return 0;
}
