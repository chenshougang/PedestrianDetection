# 行人检测-将检测结果以CAN报文的形式输出

- C++调用yolov4，使用yolov4官方接口，yolov4-tiny、yolov3等也可用

从官网下载yolov4.weights、yolov4-tiny.weights权重文件放在yolo文件夹下

- can-start.sh是在AGX上挂载CAN模块和设置波特率
- can-stop.sh是在AGX上candown CAN模块

## CAN报文情况

八个字节：【第0字节[00:无人，01:有人]】
