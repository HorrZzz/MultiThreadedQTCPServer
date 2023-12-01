# MultiThreadedQTCPServer
基于Qt6.6.0，使用QTcpServer、QTcpSocket、QThread等实现的多线程tcp服务端，可实现接受来自各个客户端的消息、给各个客户端发送消息等


- [x] 给各个客户端发送数据
- [x] 接收到数据后会将接收到的数据转为字符串同步修改listwidget中对应item的文本信息
- [ ] ui界面的接收数据暂未写，但已在myServer类中预留了接收数据的接口，调试时通过Debug输出接收到的数据

![image](https://github.com/HorrZzz/MultiThreadedQTCPServer/assets/42233737/979718b0-83f3-49b0-bdf8-c269490cd4a1)

# 主界面
![image](https://github.com/HorrZzz/MultiThreadedQTCPServer/assets/42233737/c50f753b-081c-4d22-acc0-1c9903bfc28c)
