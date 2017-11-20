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

cv::Mat threshold(cv::Mat image, int th){
  cv::Mat img = image.clone();
  uchar* pixel;

  for (int j = 0; j < img.rows; ++j){
    pixel = img.ptr<uchar>(j);
    for (int i = 0; i < img.cols; ++i){

      if(pixel[i] >= th){
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
      if((pixel[i] >= th1) && (pixel[i] <= th2)){
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

cv::Mat borders_change(cv::Mat image, int value){
    cv::Mat mat;
    std::vector<std::vector<int>> matrix = mat2vector(image);

    for(int j = 0; j < image.rows; j++){
        for(int i = 0; i < image.cols; i++){
            if(i == 0){
                matrix[j][i] = value;}
            if(j == 0){
                matrix[j][i] = value;}
            if(i == image.cols-1){
                matrix[j][i] = value;}
            if(j == image.rows-1){
                matrix[j][i] = value;}
        }
    }

    mat = vector2mat(matrix);

    return mat;
}
