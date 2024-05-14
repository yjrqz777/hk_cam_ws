//
// Created by yjrqz on 2024/5/10.
//

#ifndef BUILD_HK_CLASS_H
#define BUILD_HK_CLASS_H

#include <unistd.h>
#include <rclcpp/rclcpp.hpp>
#include "hk_interfaces/srv/hk_cam_srv.hpp"
#include "std_msgs/msg/header.hpp"
#include <sensor_msgs/msg/image.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

#include "HCNetSDK.h"
#include "LinuxPlayM4.h"
#include "iniFile.h"

typedef unsigned char *PBYTE;

extern bool need_exit;
extern LONG nport;



class HK_Node : public rclcpp::Node
{

public:
    HK_Node(std::string name);
    ~HK_Node() override
    {
        std::cout << "-----" << std::endl;
    }
private:
//    LONG nport = 1;
    rclcpp::TimerBase::SharedPtr timer_;
    std::thread publish_thread_;
    uint8_t thread_flag_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_img;
    rclcpp::Service<hk_interfaces::srv::HkCamSrv>::SharedPtr  ptz_control_service;
    int PlayHandleV40 = 0;
    int lUserID = 0;
    void timer_callback();
    void publish_loop();
    void ptz_control_callback(const hk_interfaces::srv::HkCamSrv::Request::SharedPtr request,
                              const hk_interfaces::srv::HkCamSrv::Response::SharedPtr response);
//    void PsDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pPacketBuffer, DWORD nPacketSize, void *pUser);

public:
    int hk_show2();
    int GetStream();

};


#endif //BUILD_HK_CLASS_H
