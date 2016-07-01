//
//  CVWrapper.m
//  Detector
//
//  Created by Dinh Quang Hieu on 6/17/16.
//  Copyright © 2016 Dinh Quang Hieu. All rights reserved.
//

#include "CVWrapper.h"
#include "UIImage+OpenCV.h"
#include "Detector.h"


#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

@implementation CVWrapper

+ (UIImage*)processImageWithOpenCV:(UIImage*)inputImage {
    Mat mat = [inputImage CVMat];
    //std::string result = detect(mat);
    cv::Mat stitchedMat = detect (mat);
    UIImage* result =  [UIImage imageWithCVMat:stitchedMat];
    return result;
    //return  [NSString stringWithCString:result.c_str() encoding:[NSString defaultCStringEncoding]];;
}

@end