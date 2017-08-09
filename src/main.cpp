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

int main(){
   cv::Mat src;

   std::cout << "Paper 3 Application" << "\n";
   src = cv::imread("images/objetos.bmp", 1);
   src = rgb2gray(src);

   if(!src.data){
      std::cout << "Error...\n";
   }

   cv::Mat filtered = geodesic_dilation(src, erosion(src, 3));

   cv::namedWindow("Original", CV_WINDOW_AUTOSIZE);
   cv::imshow("Original", src);

   cv::namedWindow("Filtered", CV_WINDOW_AUTOSIZE);
   cv::imshow("Filtered", filtered);

   cv::waitKey();

   return 0;
}
