//
//  morphology.cpp
//  image_processing
//
//  Created by Marco Garduño on 24/07/17.
//  Copyright © 2017 Marco Garduño. All rights reserved.
//

#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include "morphology.h"
#include "functions.h"

cv::Mat dilation(cv::Mat image, int lambda){
  cv::Mat img = image.clone();
  std::vector<std::vector<int>> matrix = mat2vector(img);

  for(int k = 0; k < lambda; k++){
  // B1
  for(int j = 0; j < image.rows; j++){
    for(int i = 0; i < image.cols-1; i++){
      if(matrix[j][i] < matrix[j][i+1]){
        matrix[j][i] = matrix[j][i+1];
      }
    }
  }

  // B2
  for(int j = 0; j < image.rows-1; j++){
    for(int i = 0; i < image.cols; i++){
      if(matrix[j][i] < matrix[j+1][i]){
        matrix[j][i] = matrix[j+1][i];
      }
    }
  }

  // B3
  for(int j = 0; j < image.rows; j++){
    for(int i = image.cols - 1; i > 0; i--){
      if(matrix[j][i] < matrix[j][i-1]){
        matrix[j][i] = matrix[j][i-1];
      }
    }
  }

  // B4
  for(int j = image.rows - 1; j > 0; j--){
    for(int i = 0; i < image.cols; i++){
      if(matrix[j][i] < matrix[j-1][i]){
        matrix[j][i] = matrix[j-1][i];
      }
    }
  }

  }

  img = vector2mat(matrix);

  return img;
}

cv::Mat erosion(cv::Mat image, int lambda){
  cv::Mat img = image.clone();

  img = negative_gray(img);
  img = dilation(img, lambda);
  img = negative_gray(img);

  return img;
}

cv::Mat opening(cv::Mat image, int lambda){
  cv::Mat img = image.clone();

  img = erosion(img, lambda);
  img = dilation(img, lambda);

  return img;
}

cv::Mat closing(cv::Mat image, int lambda){
  cv::Mat img = image.clone();

  img = dilation(img, lambda);
  img = erosion(img, lambda);

  return img;
}

cv::Mat sequential_1(cv::Mat image, int lambda){
  cv::Mat img = image.clone();

  for(int i = 0; i < lambda; i++){
    img = opening(img, i+1);
    img = closing(img, i+1);
  }

  return img;
}

cv::Mat sequential_2(cv::Mat image, int lambda){
  cv::Mat img = image.clone();

  for(int i = 0; i < lambda; i++){
    img = closing(img, i+1);
    img = opening(img, i+1);
  }

  return img;
}

cv::Mat white_top_hat(cv::Mat image, int lambda){
  cv::Mat img_original = image.clone();
  cv::Mat img = image.clone();

  img = opening(img, lambda);
  img = difference(img_original, img);

  return img;
}

cv::Mat black_top_hat(cv::Mat image, int lambda){
  cv::Mat img_original = image.clone();
  cv::Mat img = image.clone();

  img = closing(img, lambda);
  img = difference(img, img_original);

  return img;
}

cv::Mat gradient(cv::Mat image, int lambda){
  cv::Mat img = image.clone();
  cv::Mat img2 = image.clone();

  img = dilation(img, lambda);
  img2 = erosion(img2, lambda);
  img = difference(img, img2);

  return img;
}

cv::Mat external_gradient(cv::Mat image, int lambda){
  cv::Mat img = image.clone();
  cv::Mat img2 = image.clone();

  img = dilation(img, lambda);
  img = difference(img, img2);

  return img;
}

cv::Mat internal_gradient(cv::Mat image, int lambda){
  cv::Mat img = image.clone();
  cv::Mat img2 = image.clone();

  img = erosion(img, lambda);
  img = difference(img2, img);

  return img;
}

cv::Mat geodesic_dilation(cv::Mat I, cv::Mat J){
    std::vector<std::vector<int>> matrix_I = mat2vector(I);
    std::vector<std::vector<int>> matrix_J = mat2vector(J);
    std::vector<std::vector<int>> matrix_aux = matrix_I;
    std::vector <int> list_1, list_2;

    cv::Mat mat_aux;
    cv::Mat dif;

    bool flag = true;
    double maxVal;

    while(flag){
        matrix_aux = matrix_J;

        for(int j = 1; j < I.rows; j++){
            for(int i = 1; i < I.cols-1; i++){
                list_1 = { matrix_J[j-1][i-1], matrix_J[j-1][i], matrix_J[j-1][i+1],
                         matrix_J[j][i-1],   matrix_J[j][i]};
                auto max_ptr_1 = std::max_element(std::begin(list_1), std::end(list_1));
                matrix_J[j][i] = std::min(*max_ptr_1, matrix_I[j][i]);
            }
        }

        for(int j = I.rows-2; j >= 0 ; j--){
            for(int i = I.cols-2; i >= 1; i--){
                list_2 = {                    matrix_J[j][i],    matrix_J[j][i+1],
                        matrix_J[j+1][i-1], matrix_J[j+1][i],  matrix_J[j+1][i+1]};
                auto max_ptr_2 = std::max_element(std::begin(list_2), std::end(list_2));
                matrix_J[j][i] = std::min(*max_ptr_2, matrix_I[j][i]);
            }
        }

        J = vector2mat(matrix_J);
        mat_aux = vector2mat(matrix_aux);
        dif = difference(mat_aux, J);

        double min, max;
        cv::minMaxLoc(dif, &min, &max);

        if(max == 0){
            flag = false;
        }
    }

    return J;
}

cv::Mat geodesic_erosion(cv::Mat I, cv::Mat J){
  I = negative_gray(I);
  J = negative_gray(J);

  J = geodesic_dilation(I, J);

  I = negative_gray(I);
  J = negative_gray(J);

  return J;
}

cv::Mat opening_by_reconstruction(cv::Mat image, int lambda){
  cv::Mat mat_aux = image.clone();
  cv::Mat Y = image.clone();
  cv::Mat eroded = image.clone();
  cv::Mat J;

  Y = erosion(image, lambda);
  J = geodesic_dilation(mat_aux, Y);

  return J;
}

cv::Mat closing_by_reconstruction(cv::Mat image, int lambda){
  cv::Mat mat_aux = image.clone();
  cv::Mat Y = image.clone();
  cv::Mat dilated = image.clone();
  cv::Mat J;

  Y = dilation(image, lambda);
  J = geodesic_erosion(mat_aux, Y);

  return J;
}

cv::Mat sequential_reconstruction1(cv::Mat image, int lambda){
  cv::Mat mat_aux = image.clone();

  for(int i = 0; i < lambda+1; i++){
    mat_aux = opening_by_reconstruction(mat_aux, i);
    mat_aux = closing_by_reconstruction(mat_aux, i);
  }

  return mat_aux;
}

cv::Mat sequential_reconstruction2(cv::Mat image, int lambda){
  cv::Mat mat_aux = image.clone();

  for(int i = 0; i < lambda+1; i++){
    mat_aux = closing_by_reconstruction(mat_aux, i);
    mat_aux = opening_by_reconstruction(mat_aux, i);
  }

  return mat_aux;
}


cv::Mat maxima(cv::Mat image){
  cv::Mat img = image.clone();
  cv::Mat mat_aux;
  std::vector<std::vector<int>> matrix_aux = mat2vector(image);

  for(int j = 1; j < image.rows-1; j++){
    for(int i = 1; i < image.cols-1; i++){
      if(matrix_aux[j][i] > 0){
        matrix_aux[j][i] = matrix_aux[j][i] - 1;
      }
    }
  }

  mat_aux = vector2mat(matrix_aux);
  cv::Mat geo_dil = geodesic_dilation(image, mat_aux);

  cv::Mat dif = difference(image, geo_dil);
  //img = threshold(dif, 1, 255);
  img = threshold(dif, 1);

  return img;
}

cv::Mat minima(cv::Mat image){
  cv::Mat img = image.clone();
  img = negative_gray(img);
  img = maxima(img);

  return img;
}

// Modified Closing by Reconstruction - Start

int min_no_zero(std::vector<int> list){
  int min = 255;

  if(list[0] < 255 && list[0] > 0){
    min = list[0];
  }
  if(list[1] < 255 && list[1] > 0){
    min = list[1];
  }

  return min;
}

int max_no_255(std::vector<int> list){
  int max = 0;

  if(list[0] < 255 && list[0] > 0){
    max = list[0];
  }
  if(list[1] < 255 && list[1] > 0){
    max = list[1];
  }

  return max;
}

int noise_counter(cv::Mat image){
  std::vector<std::vector<int>> matrix_aux = mat2vector(image);
  int counter = 0;

  for(int j = 0; j < image.rows; j++){
    for(int i = 0; i < image.cols; i++){
      if(matrix_aux[j][i] == 0){
        counter++;
      }
    }
  }

  return counter;
}

cv::Mat comparator(cv::Mat f, cv::Mat g){
  std::vector<std::vector<int>> matrix_aux = mat2vector(f);
  std::vector<std::vector<int>> matrix_f = mat2vector(f);
  std::vector<std::vector<int>> matrix_g = mat2vector(g);

  for(int j = 0; j < f.rows; j++){
    for(int i = 0; i < f.cols; i++){
      if(matrix_f[j][i] > 0){
        matrix_aux[j][i] = matrix_f[j][i];
      }
      else{
        matrix_aux[j][i] = matrix_g[j][i];
      }
    }
  }

  cv::Mat mat_aux = vector2mat(matrix_aux);

  return mat_aux;
}

cv::Mat geodesic_dilationM(cv::Mat I, cv::Mat J){
    std::vector<std::vector<int>> matrix_I = mat2vector(I);
    std::vector<std::vector<int>> matrix_J = mat2vector(J);
    std::vector<std::vector<int>> matrix_aux = matrix_I;
    std::vector <int> list_1, list_2;

    cv::Mat mat_aux;
    cv::Mat dif;

    bool flag = true;
    double maxVal;

    while(flag){
        matrix_aux = matrix_J;

        for(int j = 1; j < I.rows; j++){
            for(int i = 1; i < I.cols-1; i++){
                list_1 = { matrix_J[j-1][i-1], matrix_J[j-1][i], matrix_J[j-1][i+1],
                         matrix_J[j][i-1],   matrix_J[j][i]};
                auto max_ptr_1 = std::max_element(std::begin(list_1), std::end(list_1));
                matrix_J[j][i] = min_no_zero({*max_ptr_1, matrix_I[j][i]});
            }
        }

        for(int j = I.rows-2; j >= 0 ; j--){
            for(int i = I.cols-2; i >= 1; i--){
                list_2 = {                    matrix_J[j][i],    matrix_J[j][i+1],
                        matrix_J[j+1][i-1], matrix_J[j+1][i],  matrix_J[j+1][i+1]};
                auto max_ptr_2 = std::max_element(std::begin(list_2), std::end(list_2));
                matrix_J[j][i] = min_no_zero({*max_ptr_2, matrix_I[j][i]});
            }
        }

        J = vector2mat(matrix_J);
        mat_aux = vector2mat(matrix_aux);
        dif = difference(mat_aux, J);

        double min, max;
        cv::minMaxLoc(dif, &min, &max);

        if(max == 0){
            flag = false;
        }
    }

    return J;
}

cv::Mat geodesic_erosionM(cv::Mat I, cv::Mat J){

  I = negative_gray(I);
  J = negative_gray(J);

  J = geodesic_dilationM(I, J);

  I = negative_gray(I);
  J = negative_gray(J);

  return J;
}

cv::Mat closing_by_reconstructionM(cv::Mat image, int lambda){
  cv::Mat mat_aux = image.clone();
  cv::Mat Y = image.clone();
  cv::Mat dilated = image.clone();
  cv::Mat J;

  Y = dilation(image, lambda);
  J = geodesic_erosionM(mat_aux, Y);

  return J;
}

cv::Mat opening_by_reconstructionM(cv::Mat image, int lambda){
  cv::Mat mat_aux = image.clone();
  cv::Mat Y = image.clone();
  cv::Mat eroded = image.clone();
  cv::Mat J;

  Y = erosion(image, lambda);
  J = geodesic_dilationM(mat_aux, Y);

  return J;
}

// Modified Closing by Reconstruction - End
