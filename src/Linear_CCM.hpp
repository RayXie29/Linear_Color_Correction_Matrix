#ifndef Linear_CCM_hpp
#define Linear_CCM_hpp

#include <opencv2/opencv.hpp>

//Linear Color Correction
void LCC(cv::Mat &Src,cv::Mat &Dst);
void LCC_CMC(cv::Mat &Src);
#endif
