//
//  color.h
//  image_processing
//
//  Created by Marco Garduño on 06/11/17.
//  Copyright © 2017 Marco Garduño. All rights reserved.
//

#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

cv::Mat rgb2gray(cv::Mat image);
cv::Mat split_rgb(cv::Mat image_color, int channel);
cv::Mat negative_color(cv::Mat image);

std::vector<cv::Mat> rgb2ihsl(cv::Mat image);
std::vector<cv::Mat> bivariable_histograms(std::vector<cv::Mat> ihsl_vector);

std::vector<cv::Mat> colorSegmentation(cv::Mat color_image);
cv::Mat colorAsignation(cv::Mat image_color, cv::Mat image_segmented);

#endif /* color_h */
