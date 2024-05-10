//
// Created by yjrqz on 2024/5/10.
//
#include "hk_cam/hk_class.h"
bool need_exit = true;
LONG nport = 1;



HK_Node::HK_Node(std::string name) : Node(name)
{
    RCLCPP_INFO(this->get_logger(), "hk_node init ...");
    pub_img = this->create_publisher<sensor_msgs::msg::Image>("hk_img",
                                                              10);
    ptz_control_service = this->create_service<hk_interfaces::srv::HkCamSrv>("PtzControl",
                                                           std::bind(&HK_Node::ptz_control_callback,
                                                                     this,
                                                                     std::placeholders::_1,
                                                                     std::placeholders::_2),
                                                                     rmw_qos_profile_services_default
                                                                     );
    RCLCPP_INFO(this->get_logger(), "ptz_control_service init ...");
//    timer_ = this->create_wall_timer(std::chrono::milliseconds(1),std::bind(&HK_Node::timer_callback, this));
    publish_thread_ = std::thread(&HK_Node::publish_loop, this);
}
//    HK_Node::hk_show2();


void HK_Node::publish_loop() {
    // 持续发布消息，不使用定时器
    while (rclcpp::ok()) {
        HK_Node::hk_show2();

        // 简单的休眠以减少发布频率（可选）
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}



void HK_Node::timer_callback() {
    /*设置日志级别*/
    HK_Node::hk_show2();
//    this->get_logger().set_level((rclcpp::Logger::Level)RCUTILS_LOG_SEVERITY_DEBUG);
    std::cout<<"======================================================"<<std::endl;
//    RCLCPP_DEBUG(this->get_logger(), "我是DEBUG级别的日志，我被打印出来了!");
//    RCLCPP_INFO(this->get_logger(), "我是INFO级别的日志，我被打印出来了!");
//    RCLCPP_WARN(this->get_logger(), "我是WARN级别的日志，我被打印出来了!");
//    RCLCPP_ERROR(this->get_logger(), "我是ERROR级别的日志，我被打印出来了!");
//    RCLCPP_FATAL(this->get_logger(), "我是FATAL级别的日志，我被打印出来了!");
}

void PsDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pPacketBuffer, DWORD nPacketSize, void *pUser)
{
//    printf("--回调");
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
//    int PlayHandleV40 = 0;
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
    int iUserID = NET_DVR_Login_V30("192.168.44.64", 8000, "admin", "air12345678", &struDeviceInfo);
    if (iUserID >= 0)
    {
        NET_DVR_PREVIEWINFO struPreviewInfo = {0};
        struPreviewInfo.lChannel = 1;
        struPreviewInfo.dwStreamType = 1;
        struPreviewInfo.dwLinkMode = 1;
        struPreviewInfo.bBlocked = 1;
        struPreviewInfo.hPlayWnd = NULL;
        struPreviewInfo.bPassbackRecord = 0;
        this->PlayHandleV40 = NET_DVR_RealPlay_V40(iUserID, &struPreviewInfo, PsDataCallBack, NULL);
        if (this->PlayHandleV40 >= 0)
            printf("[GetStream]---RealPlay %d success, \n", 1, NET_DVR_GetLastError());
        else
            printf("[GetStream]---RealPlay %d failed, error = %d\n", 1, NET_DVR_GetLastError());
    }
    else
    {
        printf("[GetStream]---Login failed, error = %d\n", NET_DVR_GetLastError());
        need_exit = false;
        NET_DVR_Cleanup();
        rclcpp::shutdown();
//        exit;
    }

    return iUserID; // return iRealPlayHandle;
}
/*
    NET_DVR_PTZControl(this->PlayHandleV40,TILT_DOWN,0);
    ZOOM_IN	11	焦距变大(倍率变大)
    ZOOM_OUT	12	焦距变小(倍率变小)
    FOCUS_NEAR	13	焦点前调
    FOCUS_FAR	14	焦点后调
    TILT_UP	21	云台上仰
    TILT_DOWN	22	云台下俯
    PAN_LEFT	23	云台左转
    PAN_RIGHT	24	云台右转
    云台停止动作或开始动作：0－开始，1－停止
    TRUE表示成功，FALSE表示失败
 */
void HK_Node::ptz_control_callback(const hk_interfaces::srv::HkCamSrv::Request::SharedPtr request,
                          const hk_interfaces::srv::HkCamSrv::Response::SharedPtr response)
{
    RCLCPP_INFO(this->get_logger(), "request->mode = %d,request->on_off = %d\n",request->mode,request->on_off);
    bool res = false;
//    ptz_cmdx = request->mode;
    if( (request->mode>=11 && request->mode<=14) ||  (request->mode>=21 && request->mode<=24) )
    {
        res = NET_DVR_PTZControl(this->PlayHandleV40,request->mode,0);
        if (res)
        {
            usleep(100);
            res = NET_DVR_PTZControl(this->PlayHandleV40,request->mode,1);
            response->success = true;
            response->errcode = 0;
            response->errtext = "NULL";
        }
    }
    else
    {
        response->success = false;
        response->errcode = 99;
        response->errtext = "err mode";
    }


}

int HK_Node::hk_show2()
{
//    sleep(1);
    DWORD dwPicSize = 1024 * 1024 * 3;
    PBYTE pJpeg2 = NULL;
    DWORD size = 0;
    BOOL ret = 0;
//    std::cout << "nport = " << nport << std::endl;
    //    for (int i = 0; i < 5000; i++)
    if (need_exit)
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

//        cv::Size size = image.size();
//        std::cout << "Width: " << size.width << ", Height: " << size.height << std::endl;

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
//        std::cout << msg.width << std::endl;
        pub_img->publish(msg);

//        char name[] = "name-%d-";
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





//int HK_Node::hk_show()
//{
//    auto msg = sensor_msgs::msg::Image();
//
//    msg.header.stamp = this->now();
//    msg.header.frame_id = "1";
//    cv::VideoCapture cap;
//    cv::Mat frame;
//    int deviceID = 0;
//    int apiID = cv::CAP_ANY;
//    cap.open(deviceID + apiID);
//    if (!cap.isOpened())
//    {
//        std::cerr << "ERROR! Unable to open camera" << std::endl;
//        return -1;
//    }
//    while (need_exit)
//    {
//        // 循环读取图片;
//        if (!cap.read(frame))
//        {
//            std::cout << "No frame" << std::endl;
//            cv::waitKey(0);
//        }
//        msg.width = frame.cols;
//        msg.height = frame.rows;
//        msg.encoding = "bgr8";
//        msg.is_bigendian = false;
//        msg.step = frame.cols * 3;
//        sensor_msgs::msg::Image::_data_type data_msg;
//        data_msg.resize(frame.total() * frame.elemSize());
//        std::memcpy(
//            data_msg.data(), frame.data,
//            frame.total() * frame.elemSize());
//        msg.data = data_msg;
//
//        pub_img->publish(msg);
//        //        std::this_thread::sleep_for(std::chrono::seconds(1));
//        // 显示图片;
//        cv::imshow("Output Window", frame);
//
//        if (cv::waitKey(1) >= 0)
//        {
//            cv::destroyAllWindows();
//            break;
//        };
//    }
//}
