from rclpy.node import Node
from protocol.srv import AudioExecute
from protocol.msg import AudioPlayExtend

import grpc
import json
import cyberdog_app_pb2
import cyberdog_app_pb2_grpc

import os
import time

def printff():
    print("1111")

mi_node = "/mi_desktop_48_b0_2d_7b_02_9c/"

class AudioT(Node):
    def __init__(self,name):
        super().__init__(name)
        self.get_audio_stat = self.create_client(AudioExecute,mi_node + "get_audio_state")   
        self.get_logger().info("self.get_audio_stat==%s"%self.get_audio_stat)
        self.pub_audio_send = self.create_publisher(AudioPlayExtend, mi_node + "speech_play_extend", 10)

    def topic_talk(self,string):
        # self.get_logger().warn('service waiting')
        while not self.get_audio_stat.wait_for_service(1):
            self.get_logger().warn('service not available, waiting again...')
        msg_send = AudioPlayExtend()
        msg_send.is_online = True
        msg_send.module_name = "AudioT"
        msg_send._speech.module_name = "AudioT"
        msg_send._speech.play_id = 32
        msg_send.text = string
        self.pub_audio_send.publish(msg_send)
        
        # self.get_logger().info("topic_talk-------")

class Client:
    def __init__(self, cyberdog_ip: str, ca_cert: str, client_key: str, client_cert: str):
        self.dog_speak = AudioT("dog_speak_my_lib")
        creds = grpc.ssl_channel_credentials(
            open(ca_cert, 'rb').read(),
            open(client_key, 'rb').read(),
            open(client_cert, 'rb').read())
        channel_options = (('grpc.ssl_target_name_override', 'cyberdog2.server'), 
                           ('grpc.default_authority', 'cyberdog2.server'))
        chennel = grpc.secure_channel(cyberdog_ip + ':50052', creds, channel_options)
        self.__stub = cyberdog_app_pb2_grpc.GrpcAppStub(chennel)
        self.dog_speak.topic_talk("Client is ready.")
        print('Client is ready.')

    def sendMsg(self, name_code, params):
        if name_code == 9999:
            self.dog_speak.destroy_node()
        else:
            try:
                requset = cyberdog_app_pb2.SendRequest(nameCode=name_code,params=params)
                result_list = self.__stub.sendMsg(requset)
                for response in result_list:
                    try: 
                        parsed_data = json.loads(response.data)
                        # print(parsed_data['feedback_code'])
                        self.analy_code(parsed_data['feedback_code'])
                    except:
                        parsed_data = json.loads(response.data)
                        print(parsed_data)
                        # print(parsed_data['feedback_code'])
            except:
                print('failed to send msg')

    def analy_code(self,feedback_code):
        if feedback_code == 300:
            print("导航启动成功，设置目标点成功，正在规划路径")
            # self.dog_speak.topic_talk("正在规划路径")
        elif feedback_code == 307:
            print("正在导航中")
            self.dog_speak.topic_talk("正在导航中")
        elif feedback_code == 308:
            print("到达目标点")
            # self.dog_speak.topic_talk("到达目标点,开始拍照请等待")
        elif feedback_code == 302:
            print("底层导航功能服务连接失败，请重新发送目标")
        elif feedback_code == 303:
            print("发送目标点失败，请重新发送目标")
        elif feedback_code == 304:
            print("底层导航功能失败，请重新发送目标")
        elif feedback_code == 305:
            print("目标点为空，请重新选择目标")
        elif feedback_code == 306:
            print("规划路径失败，请重新选择目标")
        elif feedback_code == 309:
            print("正在检查地图")
        elif feedback_code == 310:
            print("地图检查成功")
        elif feedback_code == 311:
            print("地图不存在，请重新建图")
        elif feedback_code == 1000:
            print("正在激活依赖节点")
        elif feedback_code == 1001:
            print("激活依赖节点成功")
            # self.dog_speak.topic_talk("激活依赖节点成功")
        elif feedback_code == 1002:
            print("激活依赖节点失败")
            self.dog_speak.topic_talk("激活依赖节点失败")
        else:
            print("feedback_code:{}".format(feedback_code))


