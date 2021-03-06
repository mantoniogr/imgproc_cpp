//
//  mcbr.cpp
//  image_processing
//
//  Created by Marco Garduño on 31/10/17.
//  Copyright © 2017 Marco Garduño. All rights reserved.
//

#include <iostream>
#include <vector>
#include <list>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "mcbr.h"
#include "morphology.h"
#include "functions.h"
#include "color.h"

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
        max = list[0];}
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

cv::Mat mcbr(cv::Mat image){
    cv::Mat mat_aux = image.clone();
    cv::Mat filtered = image.clone();
    int noise, size;

    size = 1;

    noise = noise_counter(image);
    // std::cout << noise << std::endl;

    mat_aux = closing(image, 1);

    noise = noise_counter(mat_aux);
    // std::cout << noise << std::endl;

    while(noise != 0){
        mat_aux = closing_by_reconstructionM(mat_aux, size);
        filtered = comparator(mat_aux, filtered);

        noise = noise_counter(mat_aux);
        // std::cout << noise << std::endl;

        size += 1;
    }

    return mat_aux;
}

cv::Mat full_inpainting(cv::Mat depth_map, cv::Mat color){
    // cv::Mat mat_aux = depth_map.clone();

    cv::Mat clos_1 = closing(depth_map, 1);
    cv::Mat mcbr_fil = mcbr(depth_map);

    // Holes isolation
    cv::Mat holes = hole_id(clos_1);
    cv::Mat holes_problems = noise_classificator(holes, mcbr_fil);
    cv::imwrite("holes_problems.png", holes_problems);

    std::vector<cv::Mat> templates_d = isolation(holes_problems, depth_map);
    cv::imwrite("templated.png", templates_d[5]);
    std::vector<cv::Mat> templates_c = isolation(holes_problems, color);
    cv::imwrite("templatec.png", templates_c[5]);

    std::vector<std::vector<cv::Point>> coords_c = template_coords(holes_problems, color);
    std::vector<std::vector<cv::Point>> coords_d = template_coords(holes_problems, depth_map);

    std::vector<int> matches_d = template_matching(depth_map, templates_d, coords_d);
    // std::vector<int> matches_c = template_matching(color, templates_c, coords_c);

    return templates_d[5];
}

std::vector<int> template_matching(cv::Mat image, std::vector<cv::Mat> templates, std::vector<std::vector<cv::Point>> template_coords){
  // Methods
  /* 0: SQDIFF
     1: SQDIFF NORMED
     2: TM CCORR
     3: TM CCORR NORMED
     4: TM COEFF
     5: TM COEFF NORMED
  */
  std::vector<int> methods = {0, 1, 2, 3, 4, 5};

  for(std::vector<int>::iterator itm = methods.begin(); itm!=methods.end(); ++itm){
    cv::Mat result;
    double threshold;
    double minVal;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;
    cv::Point matchLoc;

    cv::Mat imageAux = image.clone();

    for(int j=template_coords[1][0].y; j<template_coords[1][1].y; j++){
      for(int i=template_coords[1][0].x; i<template_coords[1][1].x; i++){
        cv::Vec3b color = imageAux.at<cv::Vec3b>(cv::Point(i,j));
        imageAux.at<cv::Vec3b>(cv::Point(i,j)) = 0;
      }
    }

    cv::matchTemplate(imageAux, templates[1], result, *itm);
    cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, CV_8UC1);
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    float dist = image.cols * image.rows;
    float dist_point = 0;
    cv::Point coords;

    if(*itm < 4){
      threshold = 0.95;
      std::vector<cv::Point> points = findValue(result, threshold, 0);
      for(int i = 0; i < points.size(); i++){
        dist_point = sqrt( pow((points[i].x - template_coords[1][0].x),2) + pow((points[i].y - template_coords[1][0].y),2) );
        if (dist_point < dist){
          dist = dist_point;
          coords = points[i];
        }
      }
      std::cout << dist_point << std::endl;
      std::cout << coords << std::endl;

      matchLoc = maxLoc;
    }
    if(*itm > 3){
      threshold = 0.05;
      std::vector<cv::Point> points = findValue(result, threshold, 1);
      for(int i = 0; i < points.size(); i++){
        dist_point = sqrt( pow((points[i].x - template_coords[1][0].x),2) + pow((points[i].y - template_coords[1][0].y),2) );
        if (dist_point < dist){
          dist = dist_point;
          coords = points[i];
        }
      }
      std::cout << dist_point << std::endl;
      std::cout << coords << std::endl;

      matchLoc = minLoc;
    }

    cv::rectangle(image, coords,
      cv::Point(coords.x + templates[1].cols, coords.y + templates[1].rows),
      cv::Scalar(255, 255, 255), 1, 8, 0);
    cv::imwrite("TemplateMatching.png", image);
  }

  return {0,0,0,0};
}

std::vector<std::vector<cv::Point>> template_coords(cv::Mat holes_problems, cv::Mat image){
    std::vector<std::vector<cv::Point>> coords;

    // Mark holes with problems in different gray level
    cv::Mat labelled = labeling(holes_problems, 1);

    // For each gray level, do a threshold
    double min, max;
    cv::minMaxLoc(labelled, &min, &max);

    cv::Mat hole_n;
    std::vector<std::vector<int>> hole_n_mat;

    // cv::Mat current_template;

    int min_x, min_y;
    int max_x, max_y;

    cv::Point p1, p2;

    for(int n = 1; n <= max; n++){
        hole_n = threshold(labelled, n, n);
        hole_n_mat = mat2vector(hole_n);

        min_x = holes_problems.cols;
        min_y = holes_problems.rows;

        max_x = 0;
        max_y = 0;

        for(int j = 0; j < holes_problems.rows; j++){
            for(int i = 0; i < holes_problems.cols; i++){
                if(hole_n_mat[j][i] == 255){
                    if(i < min_x){
                        min_x = i;
                    }
                    if(i > max_x){
                        max_x = i;
                    }
                    if(j < min_y){
                        min_y = j;
                    }
                    if(j > max_y){
                        max_y = j;
                    }
                }
            }
        }

        // Fill vector of Points
        p1.x = min_x - 1;
        p1.y = min_y - 1;
        p2.x = max_x + 1;
        p2.y = max_y + 1;

        coords.push_back({p1, p2});

    }

    return coords;
}

std::vector<cv::Mat> isolation(cv::Mat holes_problems, cv::Mat image){
  cv::Mat imageAux = image.clone();
  std::vector<cv::Mat> templates;

  // Mark holes with problems in different gray level
  cv::Mat labelled = labeling(holes_problems, 1);

  // For each gray level, do a threshold
  double min, max;
  cv::minMaxLoc(labelled, &min, &max);

  cv::Mat hole_n;
  std::vector<std::vector<int>> hole_n_mat;

  cv::Mat current_template;

  int min_x, min_y;
  int max_x, max_y;

  for(int n = 1; n <= max; n++){
      hole_n = threshold(labelled, n, n);
      hole_n_mat = mat2vector(hole_n);

      min_x = holes_problems.cols;
      min_y = holes_problems.rows;

      max_x = 0;
      max_y = 0;

      for(int j = 0; j < holes_problems.rows; j++){
          for(int i = 0; i < holes_problems.cols; i++){
              if(hole_n_mat[j][i] == 255){
                  if(i < min_x){
                      min_x = i;
                  }
                  if(i > max_x){
                      max_x = i;
                  }
                  if(j < min_y){
                      min_y = j;
                  }
                  if(j > max_y){
                      max_y = j;
                  }
              }
          }
      }

      // For every element segmented generate a template
      current_template = imageAux(cv::Rect(min_x-1, min_y-1, max_x-min_x+1, max_y-min_y+1));

      if(imageAux.channels() == 1){
        std::vector<std::vector<int>> template_v = mat2vector(current_template);
        for(int j = 0; j < current_template.rows; j++){
          for(int i = 0; i < current_template.cols; i++){
            if(hole_n_mat[j+min_y][i+min_x] != 255){
              template_v[j][i] = 255;
            }
          }
        }
        current_template = vector2mat(template_v);
      }
      if(imageAux.channels() == 3){
        cv::Mat matB = split_rgb(current_template, 0);
        cv::Mat matG = split_rgb(current_template, 1);
        cv::Mat matR = split_rgb(current_template, 2);

        std::vector<std::vector<int>> vecB = mat2vector(matB);
        std::vector<std::vector<int>> vecG = mat2vector(matG);
        std::vector<std::vector<int>> vecR = mat2vector(matR);

        for(int j = 0; j < current_template.rows; j++){
          for(int i = 0; i < current_template.cols; i++){
            if(hole_n_mat[j+min_y][i+min_x] != 255){
              vecB[j][i] = 255;
              vecG[j][i] = 255;
              vecR[j][i] = 255;
            }
          }
        }

        matB = vector2mat(vecB);
        matG = vector2mat(vecG);
        matR = vector2mat(vecR);

        std::vector<cv::Mat> channels;
        channels.push_back(matB);
        channels.push_back(matG);
        channels.push_back(matR);

        cv::merge(channels, current_template);
      }

      // Fill vector of templates
      templates.push_back(current_template);

  }

  return templates;
}

cv::Mat noise_classificator(cv::Mat image, cv::Mat depth_mcbr){
    double min, max;
    cv::minMaxLoc(image, &min, &max);

    // int max_value = max_gray_value();
    cv::Mat hole_n;
    cv::Mat external;
    cv::Mat internal;

    std::vector<std::vector<int>> external_mat;
    std::vector<std::vector<int>> internal_mat;

    std::vector<std::vector<int>> mcbr_mat = mat2vector(depth_mcbr);
    std::vector<std::vector<int>> holes_marked = mat2vector(image);
    std::vector<std::vector<int>> holes_gray;// = mat2vector(image);
    holes_gray.resize(image.rows, std::vector<int>(image.cols, 0));

    std::vector<int> list;

    for(int n = 1; n < max+1; n++){
        double acc1 = 0;
        double acc2 = 0;

        double k = 0;
        double l = 0;

        hole_n = threshold(image, n, n);

        external = external_gradient(hole_n, 1);
        internal = internal_gradient(hole_n, 1);

        external_mat = mat2vector(external);
        internal_mat = mat2vector(internal);

        for(int j = 0; j < depth_mcbr.rows; j++){
            for(int i = 0; i < depth_mcbr.cols; i++){
                if(external_mat[j][i] == 255){
                    acc1 = acc1 + mcbr_mat[j+1][i+1];
                    k++;
                }
                if(internal_mat[j][i] == 255){
                    acc2 = acc2 + mcbr_mat[j+1][i+1];
                    l++;
                }
            }
        }

        if(abs((acc1/k) - (acc2/l)) <= 1){
            list.push_back(n);
        }
    }

    int m;

    for(std::vector<int>::iterator it = list.begin(); it != list.end(); ++it){
        m = *it;
        // std::cout << m << std::endl;
        for(int j = 0; j < image.rows; j++){
            for(int i = 0; i < image.cols; i++){
                if(holes_marked[j][i] == m){
                    holes_gray[j][i] = 255;
                }
            }
        }
    }

    cv::Mat holes_2fill = vector2mat(holes_gray);

    return holes_2fill;
}

cv::Mat hole_id(cv::Mat image){
    cv::Mat mat_aux = image.clone();
    std::vector<std::vector<int>> matrix = mat2vector(image);

    for(int j = 0; j < image.rows; j++){
        for(int i = 0; i < image.cols; i++){
            if(matrix[j][i] == 0){
                matrix[j][i] = 255;
            }else{
                matrix[j][i] = 0;
            }
        }
    }

    mat_aux = vector2mat(matrix);
    mat_aux = borders_adding(mat_aux, 0);
    mat_aux = labeling(mat_aux, 1);
    // mat_aux = borders_removal(mat_aux);

    return mat_aux;
}
