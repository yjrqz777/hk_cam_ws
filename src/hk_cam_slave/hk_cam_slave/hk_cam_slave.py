#!/home/yjrqz/miniconda3/envs/yolov5/bin/python
import rclpy

import argparse
from rclpy.node import Node
#include "std_msgs/msg/header.hpp"
from sensor_msgs.msg import Image
from cv_bridge import CvBridge  

# from hk_interfaces.srv import hkcamsrv
from hk_interfaces.srv import HkCamSrv
# import hk_interfaces.srv

import cv2
import sys
# conda 环境所在路径
sys.path.append(f'/home/yjrqz/miniconda3/envs/yolov5/lib/python3.8/site-packages/')
import torch
import numpy as np
        # 'header': 'std_msgs/Header',
        # 'height': 'uint32',
        # 'width': 'uint32',
        # 'encoding': 'string',
        # 'is_bigendian': 'uint8',
        # 'step': 'uint32',
        # 'data': 'sequence<uint8>',

#include <sensor_msgs/msg/image.hpp>
class hk_cam_slave(Node):
    def __init__(self,name):
        super().__init__(name)
        self.get_logger().info("Hello ROS 2")   
        self.bridge = CvBridge() 
        self.hk_node = self.create_subscription(Image, 
                                                        "hk_img",
                                                        self.hk_node_callback,
                                                        10)
        self.vad_model= torch.hub.load('/media/yjrqz/anything/ubuntu20.04/linux/yolov5/','custom',path='/media/yjrqz/anything/ubuntu20.04/linux/yolov5/best2.pt', source='local')
        self.vad_model.conf = 0.6
        self.vad_model.iou = 0.4

    
    def hk_node_callback(self,hk_image):
        # self.get_logger().info("hk_node")
        self.get_logger().info("hk_image.height = {}\n\
                                hk_image.width  = {}\n\
                               hk_image.encoding  = {}".format(hk_image.height,hk_image.width,hk_image.encoding))
        
        # 假设图像是BGR8格式  
        # bridge = self.get_message_bridge('sensor_msgs/Image', 'cv2_bridge.CvImage')  
        # cv_image = bridge.convert(hk_image)  
  
        # 注意：上面的convert方法在实际ROS 2中没有直接实现，  
        # 所以我们需要手动转换数据  
  
        # 手动转换数据  
        height = hk_image.height  
        width = hk_image.width  
        encoding = hk_image.encoding  
  
        # OpenCV期望BGR格式，但ROS 2的默认是BGR8，所以我们通常不需要转换  
        # 但如果编码是其他格式（如RGB8），则需要相应地转换  
        if encoding == 'bgr8':  
            # 将字节数据转换为numpy数组  
            cv_image = np.frombuffer(hk_image.data, dtype=np.uint8).reshape(height, width, -1)  
            # OpenCV使用BGR格式，不需要额外转换  
        elif encoding == 'rgb8':  
            # 将字节数据转换为numpy数组  
            cv_image = np.frombuffer(hk_image.data, dtype=np.uint8).reshape(height, width, -1)  
            # OpenCV使用BGR格式，所以需要转换  
            cv_image = cv2.cvtColor(cv_image, cv2.COLOR_RGB2BGR)  
        else:  
            # 对于其他编码，你可能需要其他转换方法  
            print(f"Unsupported encoding: {encoding}")  
            return  
  
        # 使用OpenCV显示图像  
        results = self.vad_model(cv_image,augment=False)
        img_np_bgr = results.render()[0]
        cv2.imshow('Image window', img_np_bgr)  
        cv2.waitKey(1)  # 等待1毫秒，然后关闭窗口（如果需要持续显示，可以移除或调整此行代码）  
  

def main(args = None):
    rclpy.init(args=args)
    node = hk_cam_slave("hk_cam_slave")
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()
