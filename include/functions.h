//
//  functions.h
//  image_processing
//
//  Created by Marco Garduño on 24/07/17.
//  Copyright © 2017 Marco Garduño. All rights reserved.
//

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

cv::Mat rgb2gray(cv::Mat image);
cv::Mat negative_gray(cv::Mat image);
cv::Mat negative_color(cv::Mat image);
cv::Mat threshold(cv::Mat image, int th);
cv::Mat threshold(cv::Mat image, int th1, int th2);
cv::Mat difference(cv::Mat image1, cv::Mat image2);

std::vector<std::vector<int>> mat2vector(cv::Mat image);
cv::Mat vector2mat(std::vector<std::vector<int>> vector);

#endif /* functions_h */
