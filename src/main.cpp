#include <iostream>
#include <opencv2/opencv.hpp>
/****************************************************/
#include "Linear_CCM.hpp"
using namespace std;


int main(int argc, const char * argv[]) {
    
    cv::Mat img = cv::imread("./imgs/original_img.bmp",cv::IMREAD_COLOR);
    cv::Mat dst;
    LCC_CMC(img);
    LCC(img,dst);
    
    cv::imwrite("./imgs/result.bmp",dst);
    cv::imshow("original",img);
    cv::imshow("result",dst);
    cv::waitKey(0);
    cv::destroyAllWindows();
    
    return 0;
}



