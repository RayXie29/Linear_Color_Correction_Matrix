#include <iostream>
#include "Linear_CCM.hpp"
#include <opencv2/opencv.hpp>


void ROImouseEvent(int event, int x, int y, int flags, void *params)
{
    cv::Point *Pptr = (cv::Point*)params;
    
    if (event == CV_EVENT_LBUTTONDOWN && Pptr[0].x == -1 && Pptr[0].y == -1)
    {
        Pptr[0].x = x;
        Pptr[0].y = y;
    }
    
    if (flags == CV_EVENT_FLAG_LBUTTON)
    {
        
        Pptr[1].x = x;
        Pptr[1].y = y;
    }
    
    if (event == CV_EVENT_LBUTTONUP && Pptr[2].x == -1 && Pptr[2].y == -1)
    {
        Pptr[2].x = x;
        Pptr[2].y = y;
    }
}

void ROISelection(cv::Mat &img,cv::Mat &OriginalColor)
{
    cv::Point *corners = new cv::Point[3];
    corners[0].x = corners[0].y = -1;
    corners[1].x = corners[1].y = -1;
    corners[2].x = corners[2].y = -1;
    
    bool downFlag = false, upFlag = false;
    int ROICount = 0;
    cv::namedWindow("ROI select",cv::WINDOW_NORMAL);
    cv::imshow("ROI select", img);
    
    while (cv::waitKey(1) != 27 && ROICount < 24)
    {
        cv::setMouseCallback("ROI select", ROImouseEvent, corners);
        
        if (corners[0].x != -1 && corners[0].y != -1) { downFlag  = true; }
        if (corners[2].x != -1 && corners[2].y != -1) { upFlag  = true; }
        
        if (downFlag && !upFlag && corners[1].x != -1)
        {
            cv::Mat LocalImg = img.clone();
            cv::rectangle(LocalImg, corners[0], corners[1], cv::Scalar(0, 0, 0), 2);
            cv::imshow("ROI select", LocalImg);
        }
        
        if (downFlag && upFlag)
        {
            cv::Rect ROI;
            
            ROI.width = abs(corners[0].x - corners[2].x);
            ROI.height = abs(corners[0].y - corners[2].y);
            
            if(ROI.width < 5 && ROI.height <5)
            {
                std::cerr<<"ROI size too small, please re-crop the ROI"<<std::endl;
            }
            else
            {
                ROI.x = corners[0].x < corners[2].x ? corners[0].x : corners[2].x;
                ROI.y = corners[0].y < corners[2].y ? corners[0].y : corners[2].y;
                cv::destroyWindow("ROI");
                cv::Mat crop(img, ROI);
                
                cv::namedWindow("ROI", CV_WINDOW_NORMAL);
                cv::imshow("ROI", crop);
                
                int ROISize = crop.cols * crop.rows;
                cv::Scalar CropSum = cv::sum(crop)/ROISize ;
                
                float *OPtr = OriginalColor.ptr<float>(ROICount);
                OPtr[0] = CropSum[0];
                OPtr[1] = CropSum[1];
                OPtr[2] = CropSum[2];
                
                ROICount++;
            }
            
            corners[0].x = corners[0].y = -1;
            corners[1].x = corners[1].y = -1;
            corners[2].x = corners[2].y = -1;
            downFlag = upFlag  = false;
        }
        
    }
    cv::destroyAllWindows();
}

void LCC_CMC(cv::Mat &img)
{
    
    int i = 0, j = 0;
    
    cv::Mat ReferenceColor(24, 3, CV_32FC1, cv::Scalar(0));
    
    std::fstream infile;
    infile.open("./ref/ReferenceColor.csv", std::ios::in);
    
    if (!infile)
    {
        std::cerr << "Open the reference color file error" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    std::string textline;
    while (getline(infile, textline))
    {
        float *RefP = ReferenceColor.ptr<float>(i);
        j = 0;
        std::string::size_type pos = 0, prev_pos = 0;
        while ((pos = textline.find_first_of(',', pos)) != std::string::npos)
        {
            
            RefP[j++] = std::stof(textline.substr(prev_pos, pos - prev_pos));
            prev_pos = ++pos;
        }
        RefP[j++] = std::stof(textline.substr(prev_pos, pos - prev_pos));
        i++;
    }
    
    cv::Mat OriginalColor(24, 3, CV_32FC1, cv::Scalar(0));
    
    ROISelection(img, OriginalColor);
    
    cv::Mat O_T = OriginalColor.t();
    cv::Mat temp = O_T*OriginalColor;
    cv::Mat ColorMatrix(3, 3, CV_32FC1, cv::Scalar(0));
    ColorMatrix = temp.inv() * O_T * ReferenceColor;
    
    std::fstream CMC("./ref/LCC_CMC.csv", std::ios::out);
    
    if (!CMC)
    {
        std::cerr << "Writting the CMC file error" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    
    for (i = 0; i < 3; ++i)
    {
        float *CMCPtr = ColorMatrix.ptr<float>(i);
        for (j = 0; j < 3; ++j)
        {
            CMC << CMCPtr[j] << ",";
        }
        CMC << "\n";
    }
    
    CMC.close();
    
}


void LCC(cv::Mat &img,cv::Mat &Dst)
{
    int i = 0, j = 0;
    int channels = img.channels();
    int ImgHeight = img.rows, ImgWidth = img.cols;
    std::fstream CMC("./ref/LCC_CMC.csv", std::ios::in);
    
    if (!CMC)
    {
        std::cerr << "Open the file error" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    cv::Mat ColorMatrix(3, 3, CV_32FC1, cv::Scalar(0));
    
    std::string textline;
    while (getline(CMC, textline))
    {
        std::string::size_type pos = 0, prev_pos = 0;
        float *CMCPtr = ColorMatrix.ptr<float>(i);
        j = 0;
        while ((pos = textline.find_first_of(',', pos)) != std::string::npos)
        {
            CMCPtr[j++] = std::stof(textline.substr(prev_pos, pos - prev_pos));
            prev_pos = ++pos;
        }
        i++;
    }
    CMC.close();
    
    
    Dst = img.clone();
    
    float *CMC_1 = ColorMatrix.ptr<float>(0);
    float *CMC_2 = ColorMatrix.ptr<float>(1);
    float *CMC_3 = ColorMatrix.ptr<float>(2);
    
    for (i = 0; i < ImgHeight; ++i)
    {
        uchar *SP = img.ptr<uchar>(i);
        uchar *DP = Dst.ptr<uchar>(i);
        for (j = 0; j < ImgWidth*channels; j += 3)
        {
            DP[j] = cv::saturate_cast<uchar>(SP[j] * CMC_1[0] + SP[j + 1] * CMC_2[0] + SP[j+2] * CMC_3[0]);
            DP[j+1] = cv::saturate_cast<uchar>(SP[j] * CMC_1[1] + SP[j + 1] * CMC_2[1] + SP[j+2] * CMC_3[1]);
            DP[j+2] = cv::saturate_cast<uchar>(SP[j] * CMC_1[2] + SP[j + 1] * CMC_2[2] + SP[j+2] * CMC_3[2]);
        }
    }
    
}




