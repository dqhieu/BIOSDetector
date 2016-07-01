//
//  Detector.cpp
//  Detector
//
//  Created by Dinh Quang Hieu on 6/17/16.
//  Copyright © 2016 Dinh Quang Hieu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Detector.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;

vector<Rect> objects;
string result;

cv::Mat detectBorder(cv::Mat input) {
    
    cv::Mat bin = input.clone();
    
    resize(bin, bin, *new Size(640, 360));
    cvtColor(bin, bin, COLOR_RGB2GRAY);
    GaussianBlur(bin, bin, *new Size(3, 3), 0);
    adaptiveThreshold(bin, bin, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 50, 2);
    
    int dilation_size = 2;
    cv::Mat element1 = getStructuringElement(MORPH_RECT, *new Size(2 * dilation_size + 1, 2 * dilation_size + 1));
    dilate(bin, bin, element1);
    int erosion_size = 3;
    Mat element = getStructuringElement(MORPH_RECT, *new Size(2 * erosion_size + 1, 2 * erosion_size + 1));
    erode(bin, bin, element);
    
    GaussianBlur(bin, bin, *new Size(3, 3), 0);
    
    vector<Mat> contours = *new vector<Mat>();
    Mat hierarchy = *new Mat();
    findContours(bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    
    //sort(contours.begin(), contours.end(), customLess);
    for (int i = 0; i < contours.size(); i++) {
        for (int j = i + 1; j < contours.size(); j++) {
            if (boundingRect(contours[i]).area() < boundingRect(contours[j]).area()) {
                Mat a = contours[i];
                contours[i] = contours[j];
                contours[j] = a;
            }
        }
    }
    
    
    
    
    Rect border = boundingRect(contours[2]);
    border.x = border.x*input.cols/640 + 10;
    border.y = border.y*input.rows/360;
    
    border.width = border.width*input.cols/640 - 10;
    border.height = border.height*input.rows/360 - 40 ;
    
    return *new Mat(input, border);
    
}


cv::Mat detect(cv::Mat & img)
{
    resize(img, img, *new Size(240, 360));
    
    cvtColor(img, img, COLOR_BGR2GRAY);
    
    GaussianBlur(img, img, *new Size(9, 9), 2, 2);
    adaptiveThreshold(img, img, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 21, 2);
    int dilation_size = 1;
    Mat element1 = getStructuringElement(MORPH_RECT, *new Size(2 * dilation_size + 1, 2 * dilation_size + 1));
    dilate(img, img, element1);
    
    int erosion_size = 4;
    Mat element = getStructuringElement(MORPH_RECT, *new Size(2 * erosion_size + 1, 2 * erosion_size + 1));
    erode(img, img, element);
    
    int n = img.rows;
    int m = img.cols;
    
    
    int** mark = new int*[n];
    for (int i = 0; i < n; i++)
    {
        mark[i] = new int[m];
        for (int j = 0; j < m; j++) {
            mark[i][j] = 0;
        }
    }
    
    
    
    objects = *new vector<Rect>();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            
            Rect* rect = findBound(img, mark, i, j);
            if (rect != NULL && rect->area() > 50) {
                objects.push_back(*rect);
            }
            
        }
    }
    
    //return img;
    
    return extractDigits(img);
    
}

Rect *findBound(Mat img, int** mark, int i, int j) {
    if (mark[i][j] == 1 || img.at<bool>(i, j) == 1)
        return NULL;
    
    const int vi[] = {1, 0, 0, -1};
    const int vj[] = {0, 1, -1, 0};
    int n = img.rows;
    int m = img.cols;
    
    int* qi = new int[n*m];
    int* qj = new int[n*m];
    
    int maxi = 0, maxj = 0;
    int mini = n, minj = m;
    
    qi[0] = i;
    qj[0] = j;
    
    Rect *bound = NULL;
    int front = 0, rear = 1;
    while (front < rear) {
        int _i = qi[front];
        int _j = qj[front];
        front++;
        for (int k = 0; k < 4; ++k) {
            int ii = _i+vi[k];
            int jj = _j+vj[k];
            
            if (0 <= ii && ii < n && 0 <= jj && jj < m) {
                if (mark[ii][jj] == 0 && img.at<bool>(ii, jj) == 0) {
                    mark[ii][jj] = 1;
                    if (mini > ii) mini = ii;
                    if (minj > jj) minj = jj;
                    if (maxi < ii) maxi = ii;
                    if (maxj < jj) maxj = jj;
                    
                    rear++;
                    qi[rear] = ii;
                    qj[rear] = jj;
                }
            }
        }
    }
    
    delete[] qi;
    delete[] qj;
    
    if (maxi - mini > 40) {
        bound = new Rect(minj, mini, maxj-minj, maxi-mini);
    }
    
    return bound;
}

cv::Mat extractDigits(Mat img) {
    
    string strres = "";
    cvtColor(img, img, COLOR_GRAY2RGB);
    
    for (Rect rect:objects)
    {
        Mat tmp = *new Mat(img, rect);
        cvtColor(tmp, tmp, COLOR_RGB2GRAY);
        //string txt = getText(tmp);
        string txt = "";
        rectangle(img, rect.tl(), rect.br(), *new Scalar(255, 0, 0), 1);
        Rect *res = getRect(tmp, txt);
        strres += txt;
        /*
        for (int i = 0; i < 7; i++) {
            rectangle(img,
                      *new Point(rect.x + res[i].x, rect.y + res[i].y),
                      *new Point(rect.x + res[i].x + res[i].width, rect.y + res[i].y + res[i].height),
                      *new Scalar(0, 255, 0), 1);
        }
        */
        putText(img, txt,
                *new Point(rect.x + rect.width/2, rect.y + rect.height/2),
                FONT_ITALIC,
                1,
                *new Scalar(0, 0, 255));
        
        
    }
    
    result = strres;
    return img;

}

std::string getText(Mat input) {
    int n = input.cols;
    int m = input.rows;
    Rect *r = new Rect[7];
    
    
    r[0] = *new Rect(*new Point(0, m/6), *new Point(n/5, 5*m/6));
    r[1] = *new Rect(*new Point(n/10, 2*m/3), *new Point(4*n/10, m));
    r[2] = *new Rect(*new Point(6*n/10, 2*m/3), *new Point(9*n/10, m));
    r[3] = *new Rect(*new Point(4*n/5, m/6), *new Point(n, 5*m/6));
    r[4] = *new Rect(*new Point(6*n/10, 0), *new Point(9*n/10, m/3));
    r[5] = *new Rect(*new Point(n/10, 0), *new Point(4*n/10, m/3));
    r[6] = *new Rect(*new Point(2*n/5, m/6), *new Point(3*n/5, 5*m/6));
    
    if (m / 2.5 > n)
        return "1";
    
    printf("%d %d\n", input.rows, input.cols);
    
    double *c = new double[7];
    for (int k = 0; k < 7; k++) {
        Mat tmp = *new Mat(input, r[k]);
        double cnt = 0;
        for (int i = 0; i < tmp.rows; ++i) {
            for (int j = 0; j < tmp.cols; ++j)
                if (tmp.at<bool>(i,j) == true)
                    cnt = cnt + 1;
        }
        
        c[k] = cnt / (double)r[k].area();
        printf("%f\n", c[k]);
    }
    
    int const_led[10][7] = {
        {1, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {1, 0, 1, 1, 0, 1, 1},
        {1, 0, 0, 1, 1, 1, 1},
        {0, 1, 0, 0, 1, 1, 1},
        {1, 1, 0, 1, 1, 0, 1},
        {1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 0, 1, 1, 1, 1},
    };
    
    int digit = -1;
    double max = -1;
    for (int i = 2; i < 10; ++i)
    {
        if (i != 1) {
            double p = 1;
            for (int j = 0; j < 7; ++j)
            {
                if (const_led[i][j] == 1)
                    p *= c[j];
                else p *= (1 - c[j]);
            }
            if (max < p) {
                max = p;
                digit = i;
            }
        }
    }
    string res = std::to_string(digit);
    return res;
}

Rect *getRect(Mat input, string& text) {
    int n = input.cols;
    int m = input.rows;
    Rect *r = new Rect[7];
    
    r[0] = *new Rect(*new Point(0, 0), *new Point(n - 1, m/6));
    r[1] = *new Rect(*new Point(0, 0), *new Point(n/3, m/2));
    r[2] = *new Rect(*new Point(0, m/2), *new Point(n/3, m - 1));
    r[3] = *new Rect(*new Point(0, 5*m/6), *new Point(n - 1, m - 1));
    r[4] = *new Rect(*new Point(2*n/3, m/2), *new Point(n - 1, m - 1));
    r[5] = *new Rect(*new Point(2*n/3, 0), *new Point(n - 1, m/2));
    r[6] = *new Rect(*new Point(n/3, 2*m/5), *new Point(2*n/3, 3*m/5));
    
    //if (m / 2.5 > n)
    //    return "1";
    /*
    printf("%d %d\n", input.rows, input.cols);
    
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (input.at<bool>(i,j) == false) {
                printf("1");
            }
            else printf("0");
        }
        printf("\n");
    }
    */
    double *c = new double[7];
    for (int k = 0; k < 7; k++) {
        printf("%d %d %d\n", k, input.rows, input.cols);
        printf("%d %d %d %d %d\n", k, r[k].x, r[k].width, r[k].y, r[k].height );
        Mat tmp = *new Mat(input, r[k]);
        double cnt = 0;
        for (int i = 0; i < tmp.rows; ++i) {
            for (int j = 0; j < tmp.cols; ++j) {
                if (tmp.at<bool>(i,j) == false)
                {
                    //printf("1");
                    cnt = cnt + 1;
                } //else printf("0");
            }
            //printf("\n");
        }
        
        c[k] = cnt / (double)r[k].area();
        printf("%f\n", c[k]);
    }
    
    int const_led[10][7] = {
        {1, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {1, 0, 1, 1, 0, 1, 1},
        {1, 0, 0, 1, 1, 1, 1},
        {0, 1, 0, 0, 1, 1, 1},
        {1, 1, 0, 1, 1, 0, 1},
        {1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 0, 1, 1, 1, 1},
    };
    
    int digit = -1;
    double max = -1;
    for (int i = 0; i < 10; ++i)
    {
        if (i != 1) {
            double p = 1;
            for (int j = 0; j < 7; ++j)
            {
                if (const_led[i][j] == 1)
                    p *= c[j];
                else p *= (1 - c[j]);
            }
            if (max < p) {
                max = p;
                digit = i;
            }
        }
    }
    
    //if (m / 2.5 > n)
    //    digit = 1;
    
    text = std::to_string(digit);
    
    Rect *res = new Rect[7];
    Rect null = *new Rect(*new Point(0, 0), *new Point(0, 0));
    
    
    
    if (digit == 0) {
        res[0] = r[0];
        res[1] = r[1];
        res[2] = r[2];
        res[3] = r[3];
        res[4] = r[4];
        res[5] = r[5];
        res[6] = null;
    }
    else if (digit == 1) {
        res[0] = res[1] = res[2] = res[3] = res[4] = res[5] = res[6] = null;
    }
    else if (digit == 2) {
        res[0] = r[0];
        res[1] = null;
        res[2] = r[2];
        res[3] = r[3];
        res[4] = null;
        res[5] = r[5];
        res[6] = r[6];
    }
    else if (digit == 3) {
        res[0] = r[0];
        res[1] = null;
        res[2] = null;
        res[3] = r[3];
        res[4] = r[4];
        res[5] = r[5];
        res[6] = r[6];
    }
    else if (digit == 4) {
        res[0] = null;
        res[1] = r[1];
        res[2] = null;
        res[3] = null;
        res[4] = r[4];
        res[5] = r[5];
        res[6] = r[6];
    }
    else if (digit == 5) {
        res[0] = r[0];
        res[1] = r[1];
        res[2] = null;
        res[3] = r[3];
        res[4] = r[4];
        res[5] = null;
        res[6] = r[6];
    }
    else if (digit == 6) {
        res[0] = r[0];
        res[1] = r[1];
        res[2] = r[2];
        res[3] = r[3];
        res[4] = r[4];
        res[5] = null;
        res[6] = r[6];
    }
    else if (digit == 7) {
        res[0] = r[0];
        res[1] = null;
        res[2] = null;
        res[3] = null;
        res[4] = r[4];
        res[5] = r[5];
        res[6] = null;
    }
    else if (digit == 8) {
        res[0] = r[0];
        res[1] = r[1];
        res[2] = r[2];
        res[3] = r[3];
        res[4] = r[4];
        res[5] = r[5];
        res[6] = r[6];
    }
    else if (digit == 9) {
        res[0] = r[0];
        res[1] = null;
        res[2] = r[2];
        res[3] = r[3];
        res[4] = r[4];
        res[5] = r[5];
        res[6] = r[6];
    }
    
    return res;
    
}