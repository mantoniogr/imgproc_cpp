//
//  main.cpp
//  image_processing
//
//  Created by Marco Garduño on 24/07/17.
//  Copyright © 2017 Marco Garduño. All rights reserved.
//

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "functions.h"
#include "color.h"
#include "morphology.h"
#include "mcbr.h"

int main(){
   cv::Mat src;
   cv::Mat color;

   std::cout << "Paper 3 Application" << "\n";

   src = cv::imread("images/cones-disp2.png", 1);
   color = cv::imread("images/cones-im2.png", 1);

   src = rgb2gray(src);

   if(!src.data){
      std::cout << "Error...\n";
    }

   cv::Mat filtered = full_inpainting(src, color);

   cv::namedWindow("Filtered", CV_WINDOW_AUTOSIZE);
   cv::imshow("Filtered", filtered);

   cv::waitKey();

   return 0;
}
