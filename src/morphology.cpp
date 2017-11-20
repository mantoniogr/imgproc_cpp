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
#include <queue>

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

cv::Mat labeling(cv::Mat image, int increment){
    std::vector<std::vector<int>> matrix = mat2vector(image);

    cv::Mat mat_aux = image.clone();
    mat_aux = borders_change(mat_aux, 0);
    std::vector<std::vector<int>> matrix_aux = mat2vector(mat_aux);

    std::vector<int> coords = {0,0}; // j,i
    std::vector<int> primas = {0,0}; // j,i

    int k = 0;
    std::queue <std::vector<int>> fifo;

    for(int j = 0; j < image.rows; j++){
        for(int i = 0; i < image.cols; i++){
            if(matrix_aux[j][i] != 0){
                k += increment;

                coords[0] = j;
                coords[1] = i;

                fifo.push(coords);

                matrix_aux[j][i] = 0;
                matrix[j][i] = k;

                while(!fifo.empty()){
                    primas = fifo.front();
                    fifo.pop();
                    for(int n = primas[0]-1; n < primas[0]+2; n++){
                        for(int m = primas[1]-1; m < primas[1]+2; m++){
                            if(matrix_aux[n][m] != 0){
                                coords[0] = n;
                                coords[1] = m;
                                fifo.push(coords);
                                matrix_aux[n][m] = 0;
                                matrix[n][m] = k;
                            }
                        }
                    }
                }
            }
        }
    }

    mat_aux = vector2mat(matrix);

    return mat_aux;
}

cv::Mat watershed(cv::Mat image){
    cv::Mat mat_aux;
    cv::Mat mask = minima(image); // minimos de image
    cv::Mat imwl = labeling(mask, 1); // vertientes

    imwl = borders_change(imwl, 1000000);

    std::vector<std::vector<int>> matrix_imwl = mat2vector(imwl); // vertientes matrix
    std::vector<std::vector<int>> ime = mat2vector(image); // vertientes matrix
    std::vector<std::vector<int>> ims = mat2vector(image); // watershed matrix

    std::vector<std::queue<std::vector<int>>> fifoj (256);
    std::vector<int> coords = {0,0}; // j,i

    for(int j = 1; j < image.rows-1; j++){
        for(int i = 1; i < image.cols-1; i++){
            if(matrix_imwl[j][i] != 0){
                int ban_ = 255;
                for(int k = j-1; k < j+2; k++){
                    for(int l = i-1; l < i+2; l++){
                        ban_ = ban_ & matrix_imwl[k][l];
                    }
                }
                if(ban_ == 0){
                    fifoj[ime[j][i]].push({j,i});
                }
            }
        }
    }

    int i = 0;
    while(i != 256){
        while(!fifoj[i].empty()){
            coords = fifoj[i].front();
            fifoj[i].pop();
            for(int k = coords[0]-1; k < coords[0]+2; k++){
                for(int l = coords[1]-1; l < coords[1]+2; l++){
                    if(matrix_imwl[k][l] == 0){
                        for(int n = k-1; n < k+2; n++){
                            for(int m = l-1; m < l+2; m++){
                                if( matrix_imwl[n][m] != matrix_imwl[coords[0]][coords[1]] && matrix_imwl[n][m] != 0 && matrix_imwl[n][m] != 1000000){
                                    ims[k][l] = 255;
                                }
                                matrix_imwl[k][l] = matrix_imwl[coords[0]][coords[1]];
                                fifoj[ime[k][l]].push({k,l});
                            }
                        }
                    }
                }
            }
        }
        i++;
    }

    mat_aux = vector2mat(matrix_imwl);

    return mat_aux;
}

cv::Mat watershed(cv::Mat image, cv::Mat marker){
    cv::Mat mat_aux;
    //cv::Mat mask = minima(image); // minimos de image
    cv::Mat imwl = labeling(marker, 1); // vertientes

    imwl = borders_change(imwl, 1000000);

    std::vector<std::vector<int>> matrix_imwl = mat2vector(imwl); // vertientes matrix
    std::vector<std::vector<int>> ime = mat2vector(image); // vertientes matrix
    std::vector<std::vector<int>> ims = mat2vector(image); // watershed matrix

    std::vector<std::queue<std::vector<int>>> fifoj (256);
    std::vector<int> coords = {0,0}; // j,i

    for(int j = 1; j < image.rows-1; j++){
        for(int i = 1; i < image.cols-1; i++){
            if(matrix_imwl[j][i] != 0){
                int ban_ = 255;
                for(int k = j-1; k < j+2; k++){
                    for(int l = i-1; l < i+2; l++){
                        ban_ = ban_ & matrix_imwl[k][l];
                    }
                }
                if(ban_ == 0){
                    fifoj[ime[j][i]].push({j,i});
                }
            }
        }
    }

    int i = 0;
    while(i != 256){
        while(!fifoj[i].empty()){
            coords = fifoj[i].front();
            fifoj[i].pop();
            for(int k = coords[0]-1; k < coords[0]+2; k++){
                for(int l = coords[1]-1; l < coords[1]+2; l++){
                    if(matrix_imwl[k][l] == 0){
                        for(int n = k-1; n < k+2; n++){
                            for(int m = l-1; m < l+2; m++){
                                if( matrix_imwl[n][m] != matrix_imwl[coords[0]][coords[1]] && matrix_imwl[n][m] != 0 && matrix_imwl[n][m] != 1000000){
                                    ims[k][l] = 255;
                                }
                                matrix_imwl[k][l] = matrix_imwl[coords[0]][coords[1]];
                                fifoj[ime[k][l]].push({k,l});
                            }
                        }
                    }
                }
            }
        }
        i++;
    }

    mat_aux = vector2mat(ims);

    return mat_aux;
}

std::vector<cv::Mat> watershed(cv::Mat image, cv::Mat marker, cv::Mat dst){
    std::vector<cv::Mat> w_vector;
    cv::Mat mat_aux, mat_aux2;
    //cv::Mat mask = minima(image); // minimos de image
    cv::Mat imwl = labeling(marker, 1); // vertientes

    imwl = borders_change(imwl, 0);

    std::vector<std::vector<int>> matrix_imwl = mat2vector(imwl); // vertientes matrix
    std::vector<std::vector<int>> ime = mat2vector(image); // vertientes matrix
    std::vector<std::vector<int>> ims = mat2vector(dst); // watershed matrix

    std::vector<std::queue<std::vector<int>>> fifoj (256);
    std::vector<int> coords = {0,0}; // j,i

    for(int j = 1; j < image.rows-1; j++){
        for(int i = 1; i < image.cols-1; i++){
            if(matrix_imwl[j][i] != 0){
                int ban_ = 255;
                for(int k = j-1; k < j+2; k++){
                    for(int l = i-1; l < i+2; l++){
                        ban_ = ban_ & matrix_imwl[k][l];
                    }
                }
                if(ban_ == 0){
                    fifoj[ime[j][i]].push({j,i});
                }
            }
        }
    }

    int i = 0;
    while(i != 256){
        while(!fifoj[i].empty()){
            coords = fifoj[i].front();
            fifoj[i].pop();
            for(int k = coords[0]-1; k < coords[0]+2; k++){
                for(int l = coords[1]-1; l < coords[1]+2; l++){
                    if(l < image.cols && k < image.rows && l > -1 && k > -1 && matrix_imwl[k][l] == 0){
                        for(int n = k-1; n < k+2; n++){
                            for(int m = l-1; m < l+2; m++){
                                if(m < image.cols && n < image.rows && m > -1 && n > -1 && (matrix_imwl[n][m] != matrix_imwl[coords[0]][coords[1]]) && (matrix_imwl[n][m] != 0) && (matrix_imwl[n][m] != 1000000)){
                                    ims[k][l] = 255;
                                }
                                matrix_imwl[k][l] = matrix_imwl[coords[0]][coords[1]];
                                fifoj[ime[k][l]].push({k,l});
                            }
                        }
                    }
                }
            }
        }
        i++;
    }

    mat_aux = vector2mat(ims);
    mat_aux2 = vector2mat(matrix_imwl);

    w_vector.push_back(mat_aux);
    w_vector.push_back(mat_aux2);

    return w_vector;
}
