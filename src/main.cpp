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
#include "morphology.h"
#include "mcbr.h"

int main(){
   cv::Mat src;

   std::cout << "Paper 3 Application" << "\n";
   src = cv::imread("images/disp2.png", 1);
   src = rgb2gray(src);

   if(!src.data){
      std::cout << "Error...\n";
   }

   cv::Mat filtered = mcbr(src);

   cv::namedWindow("Original", CV_WINDOW_AUTOSIZE);
   cv::imshow("Original", src);
   cv::namedWindow("Filtered", CV_WINDOW_AUTOSIZE);
   cv::imshow("Filtered", filtered);
   //cv::imwrite("opening-3.png", filtered);

   cv::waitKey();

   return 0;
}
