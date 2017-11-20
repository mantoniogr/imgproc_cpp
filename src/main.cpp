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

   std::cout << "Paper 3 Application" << "\n";
   src = cv::imread("images/im2.png", 1);
   //src = rgb2gray(src);
   //src = threshold(src, 127);

   if(!src.data){
      std::cout << "Error...\n";
   }

   //std::vector<cv::Mat> filtered = rgb2ihsl(src);
   //std::vector<cv::Mat> histograms = bivariable_histograms(filtered);

   std::vector<cv::Mat> filtered = colorSegmentation(src);

   // cv::namedWindow("Original", CV_WINDOW_AUTOSIZE);
   // cv::imshow("Original", src);
   // cv::namedWindow("Filtered", CV_WINDOW_AUTOSIZE);
   // cv::imshow("Filtered", filtered[1]);

   cv::imwrite("ls.png", filtered[1]);

   // cv::waitKey();

   return 0;
}
