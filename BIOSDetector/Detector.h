//
//  Detector.h
//  Detector
//
//  Created by Dinh Quang Hieu on 6/17/16.
//  Copyright © 2016 Dinh Quang Hieu. All rights reserved.
//


#include <opencv2/opencv.hpp>

std::string detect(cv::Mat & images);
cv::Mat detectBorder(cv::Mat input);
cv::Rect *findBound(cv::Mat img, int** mark, int i, int j);
std::string extractDigits(cv::Mat img);
std::string getText(cv::Mat input);
cv::Rect* getRect(cv::Mat input, std::string& txt);
cv::Mat eraseDot(cv::Mat img);
cv::Rect *findDot(cv::Mat input, int** mark, int i, int j);