//
//  mcbr.cpp
//  image_processing
//
//  Created by Marco Garduño on 31/10/17.
//  Copyright © 2017 Marco Garduño. All rights reserved.
//

#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include "mcbr.h"
#include "morphology.h"
#include "functions.h"

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
    std::cout << noise << std::endl;

    mat_aux = closing(image, 1);

    noise = noise_counter(mat_aux);
    std::cout << noise << std::endl;

    while(noise != 0){
        mat_aux = closing_by_reconstructionM(mat_aux, size);
        filtered = comparator(mat_aux, filtered);

        noise = noise_counter(mat_aux);
        std::cout << noise << std::endl;

        size += 1;
    }

    return mat_aux;
}
