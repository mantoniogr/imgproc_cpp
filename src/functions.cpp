//
//  functions.cpp
//  image_processing
//
//  Created by Marco Garduño on 24/07/17.
//  Copyright © 2017 Marco Garduño. All rights reserved.
//

#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include "functions.h"

cv::Mat rgb2gray(cv::Mat image){
  cv::Mat img = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
  uchar gray;
  uchar* pixel;

  for (int j = 0; j < img.rows; ++j){
    cv::Vec3b* pixel_rgb = image.ptr<cv::Vec3b>(j);
    pixel = img.ptr<uchar>(j);
    for (int i = 0; i < img.cols; ++i){
      gray = 0.299*pixel_rgb[i][2] + 0.587*pixel_rgb[i][1] +
              0.114*pixel_rgb[i][0];
      pixel[i] = gray;
    }
  }

  return img;
}

cv::Mat negative_gray(cv::Mat image){
  cv::Mat img = image.clone();
  uchar* pixel;

  for (int j = 0; j < img.rows; ++j){
    pixel = img.ptr<uchar>(j);
    for (int i = 0; i < img.cols; ++i){
      pixel[i] = 255 - pixel[i];
    }
  }

  return img;
}

cv::Mat negative_color(cv::Mat image){
  cv::Mat img = image.clone();

  for (int j = 0; j < img.rows; ++j){
    cv::Vec3b* pixel_rgb = img.ptr<cv::Vec3b>(j);
    for (int i = 0; i < img.cols; ++i){
      pixel_rgb[i][2] = 255 - pixel_rgb[i][2];
      pixel_rgb[i][1] = 255 - pixel_rgb[i][1];
      pixel_rgb[i][0] = 255 - pixel_rgb[i][0];
    }
  }

  return img;
}

cv::Mat threshold(cv::Mat image, int th){
  cv::Mat img = image.clone();
  uchar* pixel;

  for (int j = 0; j < img.rows; ++j){
    pixel = img.ptr<uchar>(j);
    for (int i = 0; i < img.cols; ++i){
      if(pixel[i] > th){
        pixel[i] = 255;
      }
      else{
        pixel[i] = 0;
      }
    }
  }

  return img;
}

cv::Mat threshold(cv::Mat image, int th1, int th2){
  cv::Mat img = image.clone();
  uchar* pixel;

  for (int j = 0; j < img.rows; ++j){
    pixel = img.ptr<uchar>(j);
    for (int i = 0; i < img.cols; ++i){
      if((pixel[i] > th1) && (th2 > pixel[i])){
        pixel[i] = 255;
      }
      else{
        pixel[i] = 0;
      }
    }
  }

  return img;
}

cv::Mat difference(cv::Mat image1, cv::Mat image2){
  cv::Mat img;

  std::vector<std::vector<int>> matrix1 = mat2vector(image1);
  std::vector<std::vector<int>> matrix2 = mat2vector(image2);

  for(int j = 0; j < image1.rows; j++){
    for(int i = 0; i < image1.cols; i++){
      matrix1[j][i] = matrix1[j][i] - matrix2[j][i];
    }
  }

  img = vector2mat(matrix1);
  return img;
}

std::vector<std::vector<int>> mat2vector(cv::Mat image){
  std::vector<std::vector<int>> myVector;
  myVector.resize(image.rows, std::vector<int>(image.cols, 0));
  uchar* pixel;

  for (int j = 0; j < image.rows; ++j){
    pixel = image.ptr<uchar>(j);
    for (int i = 0; i < image.cols; ++i){
      myVector[j][i] = pixel[i];
    }
  }

  return myVector;
}

cv::Mat vector2mat(std::vector<std::vector<int>> vector){
  cv::Mat image = cv::Mat::zeros(vector.size(), vector[0].size(), CV_8UC1);
  uchar* pixel;
  for (int j = 0; j < vector.size(); ++j){
    pixel = image.ptr<uchar>(j);
    for (int i = 0; i < vector[0].size(); ++i){
      pixel[i] = vector[j][i];
    }
  }
  return image;
}
