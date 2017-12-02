//
//  color.cpp
//  image_processing
//
//  Created by Marco Garduño on 06/11/17.
//  Copyright © 2017 Marco Garduño. All rights reserved.
//

#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include "color.h"
#include "functions.h"
#include "morphology.h"
#include <math.h>

#define PI 3.1415926536

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

cv::Mat split_rgb(cv::Mat image_color, int channel){
  cv::Mat img = cv::Mat::zeros(image_color.rows, image_color.cols, CV_8UC1);
  uchar gray;
  uchar* pixel;

  for (int j = 0; j < img.rows; ++j){
    cv::Vec3b* pixel_rgb = image_color.ptr<cv::Vec3b>(j);
    pixel = img.ptr<uchar>(j);
    for (int i = 0; i < img.cols; ++i){
      gray = pixel_rgb[i][channel];
      pixel[i] = gray;
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

std::vector<cv::Mat> rgb2ihsl(cv::Mat image){
    std::vector<cv::Mat> ihsl_vector;
    cv::Mat s = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
    cv::Mat l = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
    cv::Mat h = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
    //uchar gray;
    uchar* pixel_s;
    uchar* pixel_l;
    uchar* pixel_h;

    double h_normed;

    std::vector<int> list;

    for (int j = 0; j < s.rows; ++j){
      cv::Vec3b* pixel_rgb = image.ptr<cv::Vec3b>(j);
      pixel_s = s.ptr<uchar>(j);
      pixel_l = l.ptr<uchar>(j);
      pixel_h = h.ptr<uchar>(j);
      for (int i = 0; i < s.cols; ++i){
        // L
        pixel_l[i] = 0.213*pixel_rgb[i][2] + 0.715*pixel_rgb[i][1] + 0.072*pixel_rgb[i][0];

        // S
        list = {pixel_rgb[i][2], pixel_rgb[i][1], pixel_rgb[i][0]};
        auto max = std::max_element(std::begin(list), std::end(list));
        auto min = std::min_element(std::begin(list), std::end(list));
        pixel_s[i] = (*max - *min);

        // H
        if(pixel_rgb[i][2] > pixel_rgb[i][1] && pixel_rgb[i][2] > pixel_rgb[i][0]){
            h_normed = float(pixel_rgb[i][1] - pixel_rgb[i][0]) / float(*max - *min);
        } else if(pixel_rgb[i][1] > pixel_rgb[i][0] && pixel_rgb[i][1] > pixel_rgb[i][2]){
            h_normed = (float(pixel_rgb[i][0] - pixel_rgb[i][2]) / float(*max - *min));
            h_normed = h_normed + 2.0;
        } else if(pixel_rgb[i][0] > pixel_rgb[i][1] && pixel_rgb[i][0] > pixel_rgb[i][2]){
            h_normed = float(pixel_rgb[i][2] - pixel_rgb[i][1]) / float(*max - *min);
            h_normed = h_normed + 4.0;
        } else{
            h_normed = 0.0;
        }

        if(h_normed < 0){
            h_normed = (h_normed + 6.0) / (6.0);
        } else{
            h_normed = (h_normed) / (6.0);
        }

        pixel_h[i] = int(h_normed * 255.0);
      }
    }

    ihsl_vector.push_back(h);
    ihsl_vector.push_back(s);
    ihsl_vector.push_back(l);

    return ihsl_vector;
}

std::vector<cv::Mat> bivariable_histograms(std::vector<cv::Mat> ihsl_vector){
    std::vector<cv::Mat> histograms;
    cv::Mat hs = cv::Mat::zeros(256, 256, CV_8UC1);
    cv::Mat ls = cv::Mat::zeros(256, 256, CV_8UC1);

    std::vector<std::vector<int>> h_vector = mat2vector(ihsl_vector[0]);
    std::vector<std::vector<int>> s_vector = mat2vector(ihsl_vector[1]);
    std::vector<std::vector<int>> l_vector = mat2vector(ihsl_vector[2]);

    std::vector<std::vector<int>> ls_vector;
    std::vector<std::vector<int>> hs_vector;
    std::vector<std::vector<double>> ls_vector_log;
    std::vector<std::vector<double>> hs_vector_log;

    ls_vector.resize(256, std::vector<int>(256, 0));
    ls_vector_log.resize(256, std::vector<double>(256, 0));
    hs_vector.resize(256, std::vector<int>(256, 0));
    hs_vector_log.resize(256, std::vector<double>(256, 0));

    double x, y;
    int xf, yf;

    for(int j = 0; j < ihsl_vector[0].rows; j++){
        for(int i = 0; i < ihsl_vector[0].cols; i++){
            ls_vector[s_vector[j][i]][l_vector[j][i]] = ls_vector[s_vector[j][i]][l_vector[j][i]] + 1;

            x = (s_vector[j][i]/2.0) * cos((h_vector[j][i] * 360.0 / 255.0) * (PI/180.0));
            y = (s_vector[j][i]/2.0) * sin((h_vector[j][i] * 360.0 / 255.0) * (PI/180.0));
            xf = 127 + x;
            yf = 127 + y;
            hs_vector[yf][xf] = hs_vector[yf][xf] + 1;
        }
    }

    double maxLSlog = 0.0;
    double maxHSlog = 0.0;

    for(int j = 0; j < 256; j++){
        for(int i = 0; i < 256; i++){
            if(ls_vector[j][i] > 0){
                ls_vector_log[j][i] = log10(double(ls_vector[j][i]));
                hs_vector_log[j][i] = log10(double(hs_vector[j][i]));
            }
        }
    }

    for(int j = 0; j < 256; j++){
        for(int i = 0; i < 256; i++){
            if(ls_vector_log[j][i] > maxLSlog){
                maxLSlog = ls_vector_log[j][i];
            }
            if(hs_vector_log[j][i] > maxHSlog){
                maxHSlog = hs_vector_log[j][i];
            }
        }
    }

    for(int j = 0; j < 256; j++){
        for(int i = 0; i < 256; i++){
            ls_vector[j][i] = (ls_vector_log[j][i] / maxLSlog) * 255.0;
            hs_vector[j][i] = (hs_vector_log[j][i] / maxHSlog) * 255.0;
        }
    }

    ls = vector2mat(ls_vector);
    hs = vector2mat(hs_vector);

    histograms.push_back(hs);
    histograms.push_back(ls);

    return histograms;
}

std::vector<cv::Mat> colorSegmentation(cv::Mat color_image){
    std::vector<cv::Mat> results;
    cv::Mat imgR_HS;
    cv::Mat imgR_LS;

    std::vector<std::vector<int>> vectorR_HS;
    std::vector<std::vector<int>> vectorR_LS;
    vectorR_LS.resize(color_image.rows, std::vector<int>(color_image.cols, 0));
    vectorR_HS.resize(color_image.rows, std::vector<int>(color_image.cols, 0));

    cv::Mat imgC = cv::Mat::zeros(256, 256, CV_8UC1);
    cv::Mat imgD = cv::Mat::zeros(256, 256, CV_8UC1);

    std::vector<cv::Mat> filtered = rgb2ihsl(color_image);
    std::vector<cv::Mat> histograms = bivariable_histograms(filtered);

    // HS
    cv::Mat closing_hs = closing(histograms[0], 2);
    cv::Mat negative_hs = negative_gray(closing_hs);
    cv::Mat alternado_hs = sequential_reconstruction2(negative_hs, 1);
    cv::Mat minima_hs = minima(alternado_hs);
    minima_hs = borders_change(minima_hs, 0);
    std::vector<cv::Mat> watershed_hs = watershed(alternado_hs, minima_hs, imgC);

    // LS
    cv::Mat closing_ls = closing(histograms[1], 2);
    cv::Mat negative_ls = negative_gray(closing_ls);
    cv::Mat alternado_ls = sequential_reconstruction2(negative_ls, 1);
    cv::Mat minima_ls = minima(alternado_ls);
    minima_ls = borders_change(minima_ls, 0);
    std::vector<cv::Mat> watershed_ls = watershed(alternado_ls, minima_ls, imgD);

    std::vector<std::vector<int>> l = mat2vector(filtered[2]);
    std::vector<std::vector<int>> s = mat2vector(filtered[1]);
    std::vector<std::vector<int>> h = mat2vector(filtered[0]);

    std::vector<std::vector<int>> w_ls = mat2vector(watershed_ls[1]);
    std::vector<std::vector<int>> w_hs = mat2vector(watershed_hs[1]);

    double x, y;
    int xf, yf;

    for(int j = 0; j < color_image.rows; j++){
        for(int i = 0; i < color_image.cols; i++){
            vectorR_LS[j][i] = w_ls[s[j][i]][l[j][i]];

            y = (s[j][i]/2.0) * sin(( h[j][i]*360.0/250.0 ) * (PI/180.0));
            x = (s[j][i]/2.0) * cos(( h[j][i]*360.0/250.0 ) * (PI/180.0));
            yf = 127 + y;
            xf = 127 + x;
            vectorR_HS[j][i] = w_hs[yf][xf];
        }
    }

    imgR_LS = vector2mat(vectorR_LS);
    imgR_HS = vector2mat(vectorR_HS);

    imgR_LS = colorAsignation(color_image, imgR_LS);
    imgR_HS = colorAsignation(color_image, imgR_HS);

    results.push_back(imgR_LS);
    results.push_back(imgR_HS);

    return results;
}

cv::Mat colorAsignation(cv::Mat image_color, cv::Mat image_segmented){
    cv::Mat img = image_color.clone();
    std::vector<std::vector<int>> matrix_color = mat2vector(image_color);
    std::vector<std::vector<int>> matrix_segmented = mat2vector(image_segmented);

    cv::Mat mat_r = split_rgb(image_color, 2);
    cv::Mat mat_g = split_rgb(image_color, 1);
    cv::Mat mat_b = split_rgb(image_color, 0);

    std::vector<std::vector<int>> vector_r = mat2vector(mat_r);
    std::vector<std::vector<int>> vector_g = mat2vector(mat_g);
    std::vector<std::vector<int>> vector_b = mat2vector(mat_b);

    int max_gray = 0;

    for(int j = 0; j < image_segmented.rows; j++){
        for(int i = 0; i < image_segmented.cols; i++){
            if(matrix_segmented[j][i] > max_gray){
                max_gray = matrix_segmented[j][i];
            }
        }
    }

    std::vector<int> levels_r(max_gray+1,0);
    std::vector<int> levels_g(max_gray+1,0);
    std::vector<int> levels_b(max_gray+1,0);
    std::vector<int> counter(max_gray+1,0);

    for(int j = 0; j < image_segmented.rows; j++){
        for(int i = 0; i < image_segmented.cols; i++){
            levels_r[matrix_segmented[j][i]] = levels_r[matrix_segmented[j][i]] + vector_r[j][i];
            levels_g[matrix_segmented[j][i]] = levels_g[matrix_segmented[j][i]] + vector_g[j][i];
            levels_b[matrix_segmented[j][i]] = levels_b[matrix_segmented[j][i]] + vector_b[j][i];

            counter[matrix_segmented[j][i]] = counter[matrix_segmented[j][i]] + 1;
        }
    }

    for(int k = 0; k < max_gray+1; k++){
        levels_r[k] = int(float(levels_r[k]) / float(counter[k]));
        levels_g[k] = int(float(levels_g[k]) / float(counter[k]));
        levels_b[k] = int(float(levels_b[k]) / float(counter[k]));
    }

    uchar* pixel;

    for (int j = 0; j < img.rows; ++j){
      cv::Vec3b* pixel_rgb = img.ptr<cv::Vec3b>(j);
      pixel = image_segmented.ptr<uchar>(j);
      for (int i = 0; i < img.cols; ++i){
          pixel_rgb[i][0] = levels_b[matrix_segmented[j][i]];
          pixel_rgb[i][1] = levels_g[matrix_segmented[j][i]];
          pixel_rgb[i][2] = levels_r[matrix_segmented[j][i]];
      }
    }

    return img;
}
