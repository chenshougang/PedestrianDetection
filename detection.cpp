#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <fstream>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>


#include "yolo_v2_class.hpp"

using namespace std;
using namespace cv;

#define GPU

int person = 0;
int have_person = 1;
int no_person = 0;


//画出检测框和相关信息
void DrawBoxes(Mat &frame, vector<string> classes, int classId, float conf, int left, int top, int right, int bottom)
{
	//画检测框
	rectangle(frame, Point(left, top), Point(right, bottom), Scalar(255, 178, 50), 3);

	//该检测框对应的类别和置信度
	string label = format("%.2f", conf);
	std::cout<< "label = "<< label << std::endl;
	std::cout<< "fucking = "<< classes[classId] << std::endl;
	if(classes[classId] == "person"){
		if(conf > 0.5){
			person = have_person;
			std::cout<< " person > 0.5" <<std::endl;
		}else{
			person = no_person;
			std::cout<< " person < 0.5" <<std::endl;
		}
	}
	if (!classes.empty())
	{
		CV_Assert(classId < (int)classes.size());
		label = classes[classId] + ":" + label;
		
	}

	//将标签显示在检测框顶部
	int baseLine;
	Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
	top = max(top, labelSize.height);
	rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
	putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 0), 1);
}


//画出检测结果
void Drawer(Mat &frame, vector<bbox_t> outs, vector<string> classes)
{
	//获取所有最佳检测框信息
	for (int i = 0; i < outs.size(); i++)
	{
		DrawBoxes(frame, classes, outs[i].obj_id, outs[i].prob, outs[i].x, outs[i].y,
			outs[i].x + outs[i].w, outs[i].y + outs[i].h);
	}
}


int main(void)
{
	
	
	// ##################### can_send ##########################
	int s;
	int nbytes;
	struct sockaddr_can addr;
	struct can_frame frame;
	struct ifreq ifr;

	const char *ifname = "can1";

	if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Error while opening socket");
		return -1;
	}

	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOCGIFINDEX, &ifr);
	
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Error in socket bind");
		return -2;
	}

	frame.can_id  = 0x123;
	frame.can_dlc = 1;
	// frame.data[0] = 0;
	// frame.data[1] = 0;

	// while(1){
	// nbytes = write(s, &frame, sizeof(struct can_frame));

	// 	printf("Wrote %d bytes\n", nbytes);

	// 	// sleep(1);
	// } 
	//######################################can_send#################################

	struct timeval tv1,tv2;
	long long T;
	string classesFile = "./yolo/coco.names";
	string modelConfig = "./yolo/yolov4.cfg";
	string modelWeights = "./yolo/yolov4.weights";

	//加载类别名
	vector<string> classes;
	ifstream ifs(classesFile.c_str());
	string line;
	while (getline(ifs, line)) classes.push_back(line);
	std::cout << "classs = " << classes[0] <<std::endl;
	//加载网络模型，0是指定第一块GPU
	Detector detector(modelConfig, modelWeights, 0);

	string mode = "video";
	// //图像
	// if (mode == "image")
	// {
	// 	Mat frame = imread("./data/test.jpg");

	// 	//开始计时
	// 	gettimeofday(&tv1, NULL);
	// 	//Mat图像转为yolo输入格式
	// 	shared_ptr<image_t> detImg = detector.mat_to_image_resize(frame);
	// 	//前向预测
	// 	vector<bbox_t> outs = detector.detect_resized(*detImg, frame.cols, frame.rows, 0.25);
	// 	//画图
	// 	Drawer(frame, outs, classes);
	// 	//结束计时
	// 	gettimeofday(&tv2, NULL);
	// 	//计算用时
	// 	T = (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000;
	// 	cout << T << "ms" <<endl;
		
	// 	imwrite("./data/result.jpg", frame);
	// }
	//视频
	
	if (mode == "video")
	{
		VideoCapture cap(0);
		
		// Size size(1920, 1080);
		// VideoWriter writer("./data/result.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, size);
		//int i=0;
		while (1) 
		{
			frame.data[0] = person;
	        // frame.data[1] = person;
			nbytes = write(s, &frame, sizeof(struct can_frame));
			if(nbytes == -1){
				std::cout<< "no can message"<<std::endl;
			}
	        printf("Wrote %d bytes\n", nbytes);
			std::cout<< "can1 send ok"<<std::endl; 

			// i++;
			// if(i==256)
			// 	break;
			Mat frame;
			cap >> frame;
			if (frame.empty()) 
				break;
			//
			// cv::imshow("hhhh",frame);
			// for(int i=0;i<1000000;i++){
			// 	for(int i=0;i<1000000;i++){
			// 		for(int i=0;i<1000000;i++){
						
			// 		}
			// 	}
			// }
			//开始计时
			gettimeofday(&tv1, NULL);
			//Mat图像转为yolo输入格式
			shared_ptr<image_t> detImg = detector.mat_to_image_resize(frame);
			//前向预测
			vector<bbox_t> outs = detector.detect_resized(*detImg, frame.cols, frame.rows, 0.25);
			//画图
			Drawer(frame, outs, classes);

			
			
			//结束计时
			gettimeofday(&tv2, NULL);
			//计算用时
			T = (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000;
			cout << T << "ms" <<endl;

			// writer << frame;
			imwrite("./data/result-004.jpg", frame);
			// frame.data[0] = person;
			

		}
		cap.release();
	}

    return 0;
}


// int main(){

// int s, nbytes;  
// struct sockaddr_can addr;  
// struct ifreq ifr;  
// struct can_frame frame[2] = {{0}};  
// s = socket(PF_CAN, SOCK_RAW, CAN_RAW);      //创建套接字  
// strcpy(ifr.ifr_name, "can0" );  
// ioctl(s, SIOCGIFINDEX, &ifr);                   //指定can0设备  
// addr.can_family = AF_CAN;  
// addr.can_ifindex = ifr.ifr_ifindex;  
// bind(s, (struct sockaddr *)&addr, sizeof(addr));            //将套接字与can0绑定  
// //禁用过滤规则，本进程不接收报文，只负责发送  
// setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);  
// //生成两个报文  
// frame[0].can_id = 0x11;  
// frame[0].can_dlc = 1;  
// frame[0].data[0] = 'Y';  
// frame[0].can_id = 0x22;  
// frame[0].can_dlc = 1;  
// frame[0].data[0] = 'N';  
// //循环发送两个报文  
// while(1){   
// nbytes = write(s, &frame[0], sizeof(frame[0]));     //发送frame[0]  
// if (nbytes != sizeof(frame[0])) {  
//                 printf("Send Error frame[0]\n!");  
//                 break;                              //发送错误，退出  
// }  
// sleep(1);

// nbytes = write(s, &frame[1], sizeof(frame[1]));     //发送frame[1]  
// if (nbytes != sizeof(frame[0])) {  
//                 printf("Send Error frame[1]\n!");  
//                 break;  
// }  
// sleep(1);  
// }
  
// close(s);  
// return 0;  
// }  







// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>

// #include <net/if.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <sys/ioctl.h>

// #include <linux/can.h>
// #include <linux/can/raw.h>

// int
// main(void)
// {
// 	int s;
// 	int nbytes;
// 	struct sockaddr_can addr;
// 	struct can_frame frame;
// 	struct ifreq ifr;

// 	const char *ifname = "can1";

// 	if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
// 		perror("Error while opening socket");
// 		return -1;
// 	}

// 	strcpy(ifr.ifr_name, ifname);
// 	ioctl(s, SIOCGIFINDEX, &ifr);
	
// 	addr.can_family  = AF_CAN;
// 	addr.can_ifindex = ifr.ifr_ifindex;

// 	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

// 	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
// 		perror("Error in socket bind");
// 		return -2;
// 	}

// 	frame.can_id  = 0x123;
// 	frame.can_dlc = 2;
// 	frame.data[0] = 0;
// 	frame.data[1] = 1;

// 	while(1){
// 		nbytes = write(s, &frame, sizeof(struct can_frame));

// 		printf("Wrote %d bytes\n", nbytes);

// 		sleep(1);
// 	}

// 	// nbytes = write(s, &frame, sizeof(struct can_frame));

// 	// printf("Wrote %d bytes\n", nbytes);
	
// 	return 0;
// }

