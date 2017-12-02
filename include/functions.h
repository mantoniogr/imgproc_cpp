//
//  functions.h
//  image_processing
//
//  Created by Marco Garduño on 24/07/17.
//  Copyright © 2017 Marco Garduño. All rights reserved.
//

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

cv::Mat negative_gray(cv::Mat image);
cv::Mat threshold(cv::Mat image, int th);
cv::Mat threshold(cv::Mat image, int th1, int th2);
cv::Mat difference(cv::Mat image1, cv::Mat image2);

std::vector<std::vector<int>> mat2vector(cv::Mat image);
cv::Mat vector2mat(std::vector<std::vector<int>> vector);

cv::Mat borders_change(cv::Mat image, int value);
cv::Mat borders_adding(cv::Mat image, int value);
cv::Mat borders_removal(cv::Mat image);

int max_gray_value(cv::Mat image);

#endif /* functions_h */
