
import socket  
import os  
import time
import threading
class test:
    def pic_send_task(self,host, port, directory):
        print('{},{}'.format(host,port))
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:  
            # client_socket.connect((host, port))  

            # try:
            client_socket.bind((host, port))  
            # except:
            #     client_socket.connect((host, port)) 
            client_socket.listen(5)  
            print('Server is listening...')  
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

# test2 = test()
# pic_path = "/SDCARD/picture"
# pic_send = threading.Thread(name = "xxx",target=test2.pic_send_task,args=("192.168.1.170",11000, pic_path + "/"))  
# pic_send.start()
with open("/SDCARD/ip.text","r")as f:
    ip = f.readline()
    print(ip)