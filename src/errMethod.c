#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include<time.h>
#include <stdbool.h>
#include <ctype.h>

#include "errMethod.h"

double errMeasure(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels, int method) {
    switch(method) {
        case 1:
            return computeVariance(image, x, y, blockWidth, blockHeight, width, channels);
        case 2:
            return computeMAD(image, x, y, blockWidth, blockHeight, width, channels);
        case 3:
            return computeMPD(image, x, y, blockWidth, blockHeight, width, channels);
        case 4:
            return computeEntropy(image, x, y, blockWidth, blockHeight, width, channels);
        default:
            return computeVariance(image, x, y, blockWidth, blockHeight, width, channels);
    }
}


double computeVariance(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels) {
    double mean[3] = {0, 0, 0};
    int banyakPiksel = blockWidth * blockHeight;

    for (int i = 0; i < blockHeight; i++) {
        for (int j = 0; j < blockWidth; j++) {
            int index = ((y + i) * width + (x + j)) * channels;
            for (int c = 0; c < channels; c++) {
                mean[c] += image[index + c];
            }
        }
    }

    for (int c = 0; c < channels; c++) {
        mean[c] /= banyakPiksel;
    }

    double variance[3] = {0, 0, 0};
    for (int i = 0; i < blockHeight; i++) {
        for (int j = 0; j < blockWidth; j++) {
            int index = ((y + i) * width + (x + j)) * channels;
            for (int c = 0; c < channels; c++) {
                variance[c] += pow(image[index + c] - mean[c], 2);
            }
        }
    }

    double hasil = (variance[0] + variance[1] + variance[2]) / (3 * banyakPiksel);
    return hasil;
}

double computeMAD(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels) {
    double mean[3] = {0, 0, 0};
    int banyakPiksel = blockWidth * blockHeight;

    for (int i = 0; i < blockHeight; i++) {
        for (int j = 0; j < blockWidth; j++) {
            int index = ((y + i) * width + (x + j)) * channels;
            for (int c = 0; c < channels; c++) {
                mean[c] += image[index + c];
            }
        }
    }

    for (int c = 0; c < channels; c++) {
        mean[c] /= banyakPiksel;
    }

    // Menghitung deviasi absolut tiap warna
    double mad[3] = {0, 0, 0};
    for (int i = 0; i < blockHeight; i++) {
        for (int j = 0; j < blockWidth; j++) {
            int index = ((y + i) * width + (x + j)) * channels;
            for (int c = 0; c < channels; c++) {
                mad[c] += fabs(image[index + c] - mean[c]);
            }
        }
    }

    double hasil = (mad[0] + mad[1] + mad[2]) / (3 * banyakPiksel);
    return hasil;
}

double computeMPD(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels) {
    
    int banyakPiksel = blockWidth * blockHeight;


    int minVal[3] = {255, 255, 255};
    int maxVal[3] = {0, 0, 0};

    for (int i = 0; i < blockHeight; i++) {
        for (int j = 0; j < blockWidth; j++) {
            int index = ((y + i) * width + (x + j)) * channels;

            for (int c = 0; c < 3; c++) { 
                int val = image[index + c];
                if (val < minVal[c]) minVal[c] = val;
                if (val > maxVal[c]) maxVal[c] = val;
            }
        }
    }

    double diffR = maxVal[0] - minVal[0];
    double diffG = maxVal[1] - minVal[1];
    double diffB = maxVal[2] - minVal[2];

    double hasil = (diffR + diffG + diffB) / 3.0;
    return hasil;
}

double computeEntropy(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels) {
    int banyakPiksel = blockWidth * blockHeight;


    int histogram[3][256] = {0};


    for (int i = 0; i < blockHeight; i++) {
        for (int j = 0; j < blockWidth; j++) {
            int index = ((y + i) * width + (x + j)) * channels;
            for (int c = 0; c < 3; c++) { 
                unsigned char val = image[index + c];
                histogram[c][val]++;
            }
        }
    }

    double entropy[3] = {0.0, 0.0, 0.0};
    for (int c = 0; c < 3; c++) {
        for (int i = 0; i < 256; i++) {
            if (histogram[c][i] > 0) {
                double p = (double)histogram[c][i] / banyakPiksel;
                entropy[c] -= p * log2(p);
            }
        }
    }


    return (entropy[0] + entropy[1] + entropy[2]) / 3.0;
}



// double hitungSSIM(unsigned char *image1, unsigned char *image2, int width, int height) {// ssim antara 2 gambar
//     double C1 = 6.5025; // C1 = (K1 * L)^2, dengan L = 255 dan K1 = 0.01
//     double C2 = 58.5225; // C2 = (K2 * L)^2, dengan K2 = 0.03

//     double ssim_R = 0.0, ssim_G = 0.0, ssim_B = 0.0;
//     int banyakPiksel = width * height;


//     double sum_x[3] = {0};
//     double sum_y[3] = {0};
//     double rata_x[3];
//     double rata_y[3];
//     double sigma_x2[3] = {0};
//     double sigma_y2[3] = {0};
//     double sigma_xy[3] = {0};

//     // Loop baris-kolom gambar
//     for (int i = 0; i < height; i++) {
//         for (int j = 0; j < width; j++) {
//             int idx = (i * width + j) * 3;
//             for (int c = 0; c < 3; c++) {
//                 double x_val = image1[idx + c];
//                 double y_val = image2[idx + c];
//                 sum_x[c] += x_val;
//                 sum_y[c] += y_val;
//             }
//         }
//     }


//     for (int c = 0; c < 3; c++) {
//         rata_x[c] = sum_x[c] / banyakPiksel;
//         rata_y[c] = sum_y[c] / banyakPiksel;
//     }

//     // Hitung variansi dan kovarians
//     for (int i = 0; i < height; i++) {
//         for (int j = 0; j < width; j++) {
//             int idx = (i * width + j) * 3;
//             for (int c = 0; c < 3; c++) {
//                 double x_val = image1[idx + c];
//                 double y_val = image2[idx + c];
//                 sigma_x2[c] += (x_val - rata_x[c]) * (x_val - rata_x[c]);
//                 sigma_y2[c] += (y_val - rata_y[c]) * (y_val - rata_y[c]);
//                 sigma_xy[c] += (x_val - rata_x[c]) * (y_val - rata_y[c]);
//             }
//         }
//     }


//     for (int c = 0; c < 3; c++) {
//         sigma_x2[c] /= banyakPiksel;
//         sigma_y2[c] /= banyakPiksel;
//         sigma_xy[c] /= banyakPiksel;
//     }

//     // Hitung SSIM R,G,B
//     for (int c = 0; c < 3; c++) {
//         double numerator = (2 * rata_x[c] * rata_y[c] + C1) * (2 * sigma_xy[c] + C2);
//         double denominator = (rata_x[c]*rata_x[c] + rata_y[c]*rata_y[c] + C1) * (sigma_x2[c] + sigma_y2[c] + C2);
//         double ssim = numerator / denominator;

//         if (c == 0) ssim_R = ssim;
//         else if (c == 1) ssim_G = ssim;
//         else ssim_B = ssim;
//     }

//     // Hitung total SSIM RGB
//     double wR = 0.2989, wG = 0.5870, wB = 0.1140;
//     double ssimRGB = wR * ssim_R + wG * ssim_G + wB * ssim_B;
//     return ssimRGB;
// }