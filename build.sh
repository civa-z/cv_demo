#g++ -std=c++11 main.cpp -L/usr/local/lib/ -lopencv_highgui -lopencv_core -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc
g++ -std=c++11 main.cpp affine_transformation.c -L/usr/local/lib/ -lopencv_highgui -lopencv_core -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc -I/usr/local/Cellar/opencv/4.1.0_2/include/opencv4/ -L/usr/local/Cellar/opencv/4.1.0_2/lib
