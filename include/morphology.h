//
//  morphology.h
//  image_processing
//
//  Created by Marco Garduño on 24/07/17.
//  Copyright © 2017 Marco Garduño. All rights reserved.
//

#ifndef MORPHOLOGY_H_INCLUDED
#define MORPHOLOGY_H_INCLUDED

cv::Mat dilation(cv::Mat image, int lambda);
cv::Mat erosion(cv::Mat image, int lambda);
cv::Mat opening(cv::Mat image, int lambda);
cv::Mat closing(cv::Mat image, int lambda);
cv::Mat sequential_1(cv::Mat image, int lambda);
cv::Mat sequential_2(cv::Mat image, int lambda);

cv::Mat white_top_hat(cv::Mat image, int lambda);
cv::Mat black_top_hat(cv::Mat image, int lambda);
cv::Mat gradient(cv::Mat image, int lambda);
cv::Mat external_gradient(cv::Mat image, int lambda);
cv::Mat internal_gradient(cv::Mat image, int lambda);

cv::Mat geodesic_dilation(cv::Mat I, cv::Mat J);
cv::Mat geodesic_erosion(cv::Mat I, cv::Mat J);
cv::Mat opening_by_reconstruction(cv::Mat image, int lambda);
cv::Mat closing_by_reconstruction(cv::Mat image, int lambda);
cv::Mat sequential_reconstruction1(cv::Mat image, int lambda);
cv::Mat sequential_reconstruction2(cv::Mat image, int lambda);
cv::Mat maxima(cv::Mat image);
cv::Mat minima(cv::Mat image);

int min_no_zero(std::vector<int> list);
int max_no_255(std::vector<int> list);
int noise_counter(cv::Mat image);
cv::Mat comparator(cv::Mat f, cv::Mat g);
cv::Mat geodesic_dilationM(cv::Mat I, cv::Mat J);
cv::Mat geodesic_erosionM(cv::Mat I, cv::Mat J);
cv::Mat opening_by_reconstructionM(cv::Mat image, int lambda);
cv::Mat closing_by_reconstructionM(cv::Mat image, int lambda);
cv::Mat mcbr(cv::Mat image);

#endif /* morphology_h */
