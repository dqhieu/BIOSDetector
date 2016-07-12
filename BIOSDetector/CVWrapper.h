//
//  CVWrapper.h
//  Detector
//
//  Created by Dinh Quang Hieu on 6/17/16.
//  Copyright © 2016 Dinh Quang Hieu. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

#ifdef __cplusplus
#import <opencv2/opencv.hpp>
#endif

@interface CVWrapper : NSObject

+ (NSString*)processImageWithOpenCV:(UIImage*)inputImage;

@end