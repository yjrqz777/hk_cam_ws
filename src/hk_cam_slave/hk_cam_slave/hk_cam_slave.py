#!/home/yjrqz/miniconda3/envs/yolov5/bin/python
import rclpy

import argparse
from rclpy.node import Node
from sensor_msgs.msg import Image

# from protocol.srv import AudioExecute
# from protocol.msg import AudioPlayExtend
from std_srvs.srv import Empty
from std_msgs.msg import String
# from hk_interfaces.srv import hkcamsrv
from hk_interfaces.srv import HkCamSrv
from cv_bridge import CvBridge  
import shutil
# # request
# uint8 mode
# uint8 on_off
# uint8 thread_flag
# uint8 pic_num
# ---
# # response
# bool success
# uint8 errcode
# string errtext


# import hk_interfaces.srv
from rclpy.executors import MultiThreadedExecutor
import threading  
import cv2
import sys
import os
# conda 环境所在路径
# sys.path.append(f'/home/yjrqz/miniconda3/envs/yolov5/lib/python3.8/site-packages/')
# import torch
import numpy as np
import time
import json
import socket  

current_file_abspath = os.path.abspath(__file__)  
  
# 提取目录部分  
directory_path = os.path.dirname(current_file_abspath)  
  
print("当前文件绝对路径:", current_file_abspath)  
print("当前文件所在目录:", directory_path)

sys.path.append(directory_path)

# from my_Lib import *
import my_Lib 




mi_node = "/mi_desktop_48_b0_2d_7b_02_9c/"

# mi_node = ""

class ProtoEncoder():
    def __init__(self):
        pass
        # self.grpc_client = Client(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
        # self.Ptz_cam = Ptz_Camera()







#include <sensor_msgs/msg/image.hpp>
class hk_cam_slave(Node):
    def __init__(self,name):
        super().__init__(name)
        self.get_logger().info("Hello ROS 2")  
        self.dog_speak = my_Lib.AudioT("dog_speak_salve") 
        # self.pic_path = "/SDCARD/workspace/cyberdog2_ros2_galactic/picture"
        self.pic_path = "/SDCARD/picture"
        # self.dog_speak.topic_talk("哔哔")

        self.grpc_client = my_Lib.Client("127.0.0.1",
                                          "/SDCARD/workspace/cyberdog2_ros2_galactic/grpc_demo/cert/ca-cert.pem",
                                            "/SDCARD/workspace/cyberdog2_ros2_galactic/grpc_demo/cert/client-key.pem",
                                              "/SDCARD/workspace/cyberdog2_ros2_galactic/grpc_demo/cert/client-cert.pem")
        self.bridge = CvBridge() 
        self.cmd_text = ""
        self.get_text_ = self.create_subscription(String,mi_node+"asr_text",self.get_text_callback,10)
        # 停止说话
        self.stop_paly_ = self.create_client(Empty,mi_node+"stop_play")
        self.hk_node = self.create_subscription(Image, 
                                                        "hk_img",
                                                        self.hk_node_callback,
                                                        10)
        self.ptz_control_client = self.create_client(HkCamSrv,
                                                            "PtzControl",
                                                            )
        self.stop_event = threading.Event()  
        # self.my_thread = threading.Thread(target=self.thread_task)  
        # self.stop_event.set()
        # self.my_thread.start()
        # time.sleep(1)
        
        # self.get_logger().info('-----------------self.stop_event.is_set()1={}'.format(self.stop_event.is_set()))
        # print(formatted_time)
        # self.vad_model= torch.hub.load('/media/yjrqz/anything/ubuntu20.04/linux/yolov5/','custom',path='/media/yjrqz/anything/ubuntu20.04/linux/yolov5/best2.pt', source='local')
        # self.vad_model.conf = 0.6
        # self.vad_model.iou = 0.4
    def thread_task(self): 
        self.get_logger().info(f'++++++++++++++{self.stop_event.is_set()}')
        while not self.stop_event.is_set():  
            while not self.ptz_control_client.wait_for_service(timeout_sec=1.0):
                self.get_logger().info('service not available, waiting again')

            # self.req = HkCamSrv.Request()
            # self.req.mode = 1
            # self.req.on_off = 0
            # self.req.thread_flag = 0
            # self.req.pic_num = 0
            # self.req.get_pic = 1
            # reqs =  self.ptz_control_client.call_async(self.req)

            # while not reqs.done():  
            #     time.sleep(0.1) 
            # response = reqs.result()
            # self.get_logger().info("response.success = {},{}\n".format(response.success,response.errtext))
            self.read_post()

        

            # print("This is running in a separate thread.")  
            time.sleep(1)  

    def take_pic(self,name):
        self.req = HkCamSrv.Request()
        # self.req.mode = 1
        # self.req.on_off = 0
        # self.req.thread_flag = 0
        # self.req.pic_num = 0
        self.req.get_pic = 1
        self.req.get_pic_name = name
        reqs =  self.ptz_control_client.call_async(self.req)
        i=0
        while not reqs.done():  
            i=i+1
            if i>1000:
                break
            time.sleep(0.1) 
        response = reqs.result()
        self.get_logger().info("response.success = {},{}\n".format(response.success,response.errtext))

    def PTZPreset_Other(self,id):
        self.req = HkCamSrv.Request()
        self.req.mode = 39
        self.req.point_id = id
        reqs =  self.ptz_control_client.call_async(self.req)

        while not reqs.done():  
            time.sleep(0.1) 
        response = reqs.result()
        self.get_logger().info("response.success2 = {},{}\n".format(response.success,response.errcode))


    def stop_thread(self):  
        self.stop_event.set()  

    def get_text_callback(self,text):
        # self.get_logger().info("hk_node")
        self.get_logger().info(text.data)
        # self.cmd_text = text.data
        if "开始" in text.data or "巡检" in text.data or "开始巡检" in text.data:
            self.get_logger().info('-----------------self.stop_event.is_set()={}'.format(self.stop_event.is_set()))
            # self.stop_event.clear()
            try:
                self.my_thread = threading.Thread(name = "xxx",target=self.thread_task)  
                self.my_thread.start()
                self.stop_event.clear()
                # 强制闭嘴，因为开始说话有大概0.7s延迟,循环之后让他完全不出声，自行调整
                Empty2 = Empty.Request()
                for i in range(0,8):
                    time.sleep(0.1)
                    # self.get_logger().info('+1{}'.format(i))
                    self.stop_paly_.call_async(Empty2)
                self.grpc_client.dog_speak.topic_talk("开始巡检")
            except:
                # 强制闭嘴，因为开始说话有大概0.6s延迟,循环之后让他完全不出声，自行调整
                Empty2 = Empty.Request()
                for i in range(0,8):
                    time.sleep(0.1)
                    # self.get_logger().info('+2{}'.format(i))
                    self.stop_paly_.call_async(Empty2)
        else:
            # 强制闭嘴，因为开始说话有大概0.6s延迟,循环之后让他完全不出声，自行调整
            Empty2 = Empty.Request()
            for i in range(0,8):
                time.sleep(0.1)
                # self.get_logger().info('+3{}'.format(i))
                self.stop_paly_.call_async(Empty2)
            self.grpc_client.dog_speak.topic_talk(text.data)
            
        if "传输" in text.data or "图片" in text.data or "传输图片" in text.data:
            with open("/SDCARD/ip.text","r")as f:
                ip = f.readline()
            
                # print(ip)
            self.pic_send = threading.Thread(name = "xxx",target=self.pic_send_task,args=(ip,11000,self.pic_path + "/"))  
            self.pic_send.start()
            Empty2 = Empty.Request()
            for i in range(0,8):
                time.sleep(0.1)
                # self.get_logger().info('+2{}'.format(i))
                self.stop_paly_.call_async(Empty2)
            # self.my_thread.start()
            
            # self.stop_thread()
        # 强制闭嘴，因为开始说话有大概0.6s延迟,循环之后让他完全不出声，自行调整

    def pic_send_task(self,host, port, directory):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:  
            try:
                client_socket.bind((host, port))  
                # except:
                #     client_socket.connect((host, port)) 
                client_socket.listen(5)  
                # print('Server is listening...')
                time.sleep(1)
                self.grpc_client.dog_speak.topic_talk("等待上位机连接，ip地址为{}".format(host))  
                
                client_socket, addr = client_socket.accept() 
        

                files_and_directories = os.listdir(directory)  
                # print(files_and_directories)
                # 打印出所有文件和文件夹  
                for item in files_and_directories:  
                    # print(item)
                    image_path = directory + item
                    print(image_path)
                # 发送图片文件名  
                    filename = os.path.basename(image_path)  
                    print(filename)
                    time.sleep(0.1)
                    client_socket.sendall(filename.encode('utf-8'))  
            
                    # 发送文件大小（可选，帮助服务端知道何时停止接收）  
                    with open(image_path, 'rb') as f:  
                        filesize = os.path.getsize(image_path)  
                        client_socket.sendall(filesize.to_bytes(8, byteorder='big'))  
            
                    # 发送图片数据  
                    with open(image_path, 'rb') as f:  
                        while True:  
                            data = f.read(4096)  # 每次发送4096字节  
                            if not data:  
                                break  
                            client_socket.sendall(data)  
                            # print('send data')
                client_socket.close()
            except:
                self.grpc_client.dog_speak.topic_talk("上位机连接失败，请重新输入语言指令")  
    
    def read_post(self):
        # self.del_dir_file(self.pic_path)
        txt_files_os = [f for f in os.listdir("/home/mi/mapping") if f.endswith('.json')]
        # print(txt_files_os)
        with open("/home/mi/mapping/" + txt_files_os[0], 'r') as file: 
            label_num_list = []
            i = 0
            content = file.read()
            content= json.loads(content)
            label_num = len(content) - 2
            for key in content.keys():
                i = i+1
                if i>2 :
                    label_num_list.append(key)
                    print(f"{label_num_list}")
            # print(content,label_num)
            if label_num<=-1:
                self.grpc_client.dog_speak.topic_talk("地图或标签无效")
                self.stop_thread()

            else:
                for i in range(0, label_num):
                    # if i == 1:
                    #     time.sleep(1)
                    x = content[label_num_list[i]]["x"]
                    y = content[label_num_list[i]]["y"]
                    json_str = self.encodeVel(x,y)

                    self.grpc_client.sendMsg(6004, json_str)
                    # print(label_num,i,json_str)
                    
                    # print("/SDCARD/picture/{}.jpg".format(label_name))
                    formatted_time = time.strftime("%Y-%m-%d-%H-%M-%S", time.localtime())
                    print(i)
                    self.grpc_client.dog_speak.topic_talk("开始拍照请等待")
                    self.PTZPreset_Other(i)
                    time.sleep(22)
                    # print(i)
                    strings = self.pic_path+ "/" + formatted_time + "-{}.jpg".format(label_num_list[i])
                    # self.get_logger().info("----------------")
                    # self.get_logger().info("{}\n".format(strings))
                    # print(strings)
                    self.take_pic(strings)
                    # self.take_pic("/home/mi/Picture"+ "/" + formatted_time + "-{}.jpg".format(i))
                    
                    
                    # ptz.take_control_easy(i)
                    # ptz.take_control(PAN_LEFT,1)
                    # ptz.take_control(ZOOM_OUT,1)
                    # ptz.take_pic(p_size=9,p_name="{}".format(label_name))
                    self.grpc_client.dog_speak.topic_talk("拍照完成")
                    # label_name = ""
                    if i == label_num-1:
                        self.grpc_client.sendMsg(9999, json_str)
                        self.stop_thread()
                        # ptz.LogoutDev()
                    time.sleep(1)

    
    def del_dir_file(self, folder_path):
        # 遍历文件夹中的所有文件并删除它们
        for filename in os.listdir(folder_path):
            file_path = os.path.join(folder_path, filename)
            try:
                if os.path.isfile(file_path) or os.path.islink(file_path):
                    os.unlink(file_path)  # 删除文件或符号链接
                elif os.path.isdir(file_path):
                    shutil.rmtree(file_path)  # 删除子文件夹及其内容
            except Exception as e:
                print(f'Failed to delete {file_path}. Reason: {e}')


    def encodeVel(self,x,y):
        cmd = {}
        # cmd['enable'] = True
        # cmd["is_version"] = True
        cmd["type"] = 1
        # cmd["outdoor"] = False
        # cmd["map_name"] = "rue"
        cmd["goalX"] = x
        cmd["goalY"] = y
        # cmd["theta"] = -180
        return json.dumps(cmd)


    def hk_node_callback(self,hk_image):
        
        # self.get_logger().info("hk_node")
        self.get_logger().info("hk_image.height = {}\n\
                                hk_image.width  = {}\n\
                               hk_image.encoding  = {}".format(hk_image.height,hk_image.width,hk_image.encoding))
        pass
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
        # results = self.vad_model(cv_image,augment=False)
        # img_np_bgr = results.render()[0]
        # cv2.imshow('Image window', cv_image)  
        # cv2.waitKey(1)  # 等待1毫秒，然后关闭窗口（如果需要持续显示，可以移除或调整此行代码）  
  

def main(args = None):
    rclpy.init(args=args)
    node = hk_cam_slave("hk_cam_slave")

    executor = MultiThreadedExecutor()
    executor.add_node(node)
    executor.spin()

    # rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()
