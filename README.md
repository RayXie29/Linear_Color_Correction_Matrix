Usually, color correction is included in image preprocessing of camera devices. Normally this process will be done by the ISP(image signal processor) in the camera device. In this repo, the implementation of Color correction is more like a color calibration process in the beginning. We first take an image sample of colorchecker(24 color chart) which took by the our camera device. This camera device might have not tune the image quality yet(at least the color correction part), which means its ISP doesnt have suitable color correction matrix for this camera device.<br />

The image have not been corrected might be like this:<br />
![alt text](https://raw.githubusercontent.com/Gambler1993/Linear_Color_Correction_Matrix/master/imgs/original_img.bmp)


After it generates the color correction matrix by function "LCC_CMC", its ISP can use this specific color correction matrix for color correction in the future.<br />
 
The image had been correction like this:<br />
![alt text](https://raw.githubusercontent.com/Gambler1993/Linear_Color_Correction_Matrix/master/imgs/result.bmp)

The "ReferenceColor.csv" file is the reference color value we want to refer and the "LCC_CMC.csv" file is the color correction matrix.<br />
There is "ROISelection" function for taking the color value of original condition. The ROI cropping order should be "from top to the bottom, from left to the right.". It will collect 24 color values of your colorchecker image sample. 

The ROI cropping process:<br />
![alt text](https://raw.githubusercontent.com/Gambler1993/Linear_Color_Correction_Matrix/master/imgs/ROI_cropping.png)
 
If you dont like the color performance of your own camera, you can use this algorithm to correct the color either. 

	



