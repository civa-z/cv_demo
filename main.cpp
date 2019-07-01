#include <iostream>
#include <string>
#include<fstream>

#include <pthread.h>

#include "opencv2/opencv.hpp"
#include "calc_fps.hpp"
#include "statistic_time.hpp"
#include "mystring.hpp"
#include "affine_transformation.h"

const int thread_number = 1;

#if 0 // 图像移动
const float zoom_scale = 1; //图像缩放比例
const float angle = 0;      //图像旋转角度 逆时针
const float move_x = -1280 / 2;     //图像水平方向移动
const float move_y = -720 / 2;     //图像垂直方向移动
#elif 1 // 图像旋转
const float zoom_scale = 1; //图像缩放比例
const float angle = 15;     //图像旋转角度
const float move_x = 0;     //图像水平方向移动
const float move_y = 0;     //图像垂直方向移动
#elif 1 // 图像缩放
const float zoom_scale = 0.8; //图像缩放比例
const float angle = 0;     //图像旋转角度
const float move_x = 0;     //图像水平方向移动
const float move_y = 0;     //图像垂直方向移动
#endif

cv::Mat_<float> M(2,3);
cv::Mat_<float> invM(2,3);
void set_inv_trans_matrix();
void set_trans_matrix(float zoom_scale, float angle, int move_x, int move_y){
#if 1
    float alpha = angle / 180.0 * CV_PI;
    float scale = zoom_scale;
    M(0, 0) = scale * cos(alpha);
    M(0, 1) = scale * sin(alpha);
    M(1, 0) = -M(0, 1);
    M(1, 1) = M(0, 0);
    M(0, 2) = move_x;
    M(1, 2) = move_y;
#else
    M(0, 0) = 0.3535;
    M(0, 1) = -0.3535;
    M(1, 0) = 0.3535;
    M(1, 1) = 0.3535;
    M(0, 2) = 541;
    M(1, 2) = 6.5;
#endif
    set_inv_trans_matrix();
}

void set_inv_trans_matrix(){
    float D = M(0, 0) * M(1, 1) - M(0, 1) * M(1, 0);
    D = D != 0 ? 1. / D : 0;
    
    invM(0, 0) = M(1, 1) * D;
    invM(0, 1) = M(0, 1) * (-D);
    invM(1, 0) = M(1, 0) * (-D);
    invM(1, 1) = M(0, 0) * D;
    invM(0, 2) = -invM(0, 0) * M(0, 2) - invM(0, 1) * M(1, 2);
    invM(1, 2) = -invM(1, 0) * M(0, 2) - invM(1, 1) * M(1, 2);
}

void affine_trans(cv::Mat input, cv::Mat output){
    warpAffine(input, output, M, output.size(), cv::INTER_LINEAR);
}

void my_affine_trans(cv::Mat input, cv::Mat output){
    AffineTransInfo affine_trans_info;
    affine_trans_info.input_data = input.data;
    affine_trans_info.input_cols = input.cols;
    affine_trans_info.input_rows = input.rows;
    affine_trans_info.M_data = (float*)(invM.data);
    affine_trans_info.output_data = output.data;
    affine_trans_info.output_cols = output.cols;
    affine_trans_info.output_rows = output.rows;
    affine_trans_info.start_line = 0;
    affine_trans_info.end_line = output.rows;
    affine_transformation(&affine_trans_info);
}

void my_affine_trans_multithread(cv::Mat input, cv::Mat output){
    AffineTransInfo affine_trans_infos[thread_number];
    pthread_t tids[thread_number];

    int step = output.rows / thread_number;
    int add_one_number = output.rows % thread_number;

    int start_line = 0, lines;
    for (int i = 0; i < thread_number; ++i){
        if (i < add_one_number)
            lines = step + 1;
        else
            lines = step;

        affine_trans_infos[i].input_data = input.data;
        affine_trans_infos[i].input_cols = input.cols;
        affine_trans_infos[i].input_rows = input.rows;
        affine_trans_infos[i].M_data = (float*)(invM.data);
        affine_trans_infos[i].output_data = output.data;
        affine_trans_infos[i].output_cols = output.cols;
        affine_trans_infos[i].output_rows = output.rows;
        affine_trans_infos[i].start_line = start_line;
        affine_trans_infos[i].end_line = start_line+lines;
        pthread_create(&tids[i], NULL, affine_transformation, &affine_trans_infos[i]);
        start_line += lines;
    }
    void* status;
    for (int i = 0; i < thread_number; ++i)
        pthread_join(tids[i], &status);
}

int main(int argc, char** argv){
    int device_id = 0;
    if (argc != 2){
        std::cout<<"error parameter num\n";
        return -1;
    }

    std::cout<<"device: "<<argv[1]<<std::endl;
    device_id = atoi(argv[1]);

    cv::VideoCapture camera;
    camera.open(device_id);

    if (!camera.isOpened()){
        std::cout<<"Camera openfailed\n";
        return 0;
    }
    std::cout<<"Camera opened\n";

    std::ofstream f11("test.yuv");
    if(!f11){
        std::cout<<"file open error\n";
        return 0;
    }
    set_trans_matrix(zoom_scale, angle, move_x, move_y);

    
    int counter = 0;
    CalcFPS calcfps;
    StatisticTime statistictime1(100);
    StatisticTime statistictime2(100);
    while(true){
        counter++;
        cv::Mat frame;
        camera>>frame;

        if (frame.empty())
        {
	        std::cout<<"empty frame\n";
            break;
        }
        else{
           //std::cout<<"get frame "<<counter<<" "<<frame.rows<<" "<<frame.cols<<" "<<frame.type()<<"\n";
	    }
        cv::putText(frame, MyString::data_to_string(calcfps.getfps()) + "fps",
            cv::Point(10, 20), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255), 1, 8, 0);


        cv::imshow("Camera0", frame);
        cv::Mat affine_image(cv::Size(1280, 720), CV_8UC3);
        
        statistictime1.start();
        affine_trans(frame, affine_image);
        statistictime1.stop();
        cv::putText(affine_image, std::string("average time: ") + MyString::data_to_string(statistictime1.average()) + "ms",
            cv::Point(10, 60), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255), 1, 8, 0);
        cv::imshow("Camera1", affine_image);
        
        cv::Mat my_affine_image = cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3);
#if 1
        statistictime2.start();
        my_affine_trans(frame, my_affine_image);
        statistictime2.stop();
        cv::putText(my_affine_image, std::string("average time: ") + MyString::data_to_string(statistictime2.average()) + "ms",
            cv::Point(10, 60), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255), 1, 8, 0);
        cv::imshow("Camera2", my_affine_image);
#elif 1
        statistictime2.start();
        my_affine_trans_multithread(frame, my_affine_image);
        statistictime2.stop();
        cv::putText(my_affine_image, std::string("average time: ") + MyString::data_to_string(statistictime2.average()) + "ms",
            cv::Point(10, 60), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255), 1, 8, 0);
        cv::imshow("Camera2", my_affine_image);
#endif

        int c = cv::waitKey(10);
        if(c == 113)
            break;
        switch(c){
            case -1:
                break;
            default:
                std::cout<<"keyvalue: "<<c<<std::endl;
                break;
        }
    }

    return 0;
}
