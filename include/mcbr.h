//
//  mcbr.h
//  image_processing
//
//  Created by Marco Garduño on 31/10/17.
//  Copyright © 2017 Marco Garduño. All rights reserved.
//

#ifndef MCBR_H_INCLUDED
#define MCBR_H_INCLUDED

int min_no_zero(std::vector<int> list);
int max_no_255(std::vector<int> list);
int noise_counter(cv::Mat image);
cv::Mat comparator(cv::Mat f, cv::Mat g);
cv::Mat geodesic_dilationM(cv::Mat I, cv::Mat J);
cv::Mat geodesic_erosionM(cv::Mat I, cv::Mat J);
cv::Mat opening_by_reconstructionM(cv::Mat image, int lambda);
cv::Mat closing_by_reconstructionM(cv::Mat image, int lambda);
cv::Mat mcbr(cv::Mat image);
cv::Mat full_inpainting(cv::Mat depth_map, cv::Mat color);

cv::Mat noise_classificator(cv::Mat image, cv::Mat depth_mcbr);
cv::Mat hole_id(cv::Mat image);

std::vector<cv::Mat> isolation(cv::Mat holes_problems, cv::Mat depth, cv::Mat color);

#endif /* mcbr_h */
