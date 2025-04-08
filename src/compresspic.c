#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include<time.h>
#include <stdbool.h>
#include <ctype.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int maxDepth = 0;
int nodeCount = 0;

double computeVariance(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels);
double computeMAD(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels);
double computeMPD(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels);
double computeEntropy(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels);
// double computeSSIM(unsigned char* image, unsigned char* refImage, int x, int y, int blockWidth, int blockHeight, int width, int channels);


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



void normColor(unsigned char* compressed, int x, int y, int blockWidth, int blockHeight, int width, int channels, unsigned char* image) {
    int sum[3] = {0, 0, 0};
    int banyakPiksel = blockWidth * blockHeight;

    for (int i = 0; i < blockHeight; i++) {
        for (int j = 0; j < blockWidth; j++) {
            int index = ((y + i) * width + (x + j)) * channels;
            for (int c = 0; c < channels; c++) {
                sum[c] += image[index + c];
            }
        }
    }

    for (int c = 0; c < channels; c++) {
        sum[c] /= banyakPiksel;
    }

    for (int i = 0; i < blockHeight; i++) {
        for (int j = 0; j < blockWidth; j++) {
            int index = ((y + i) * width + (x + j)) * channels;
            for (int c = 0; c < channels; c++) {
                compressed[index + c] = sum[c];
            }
        }
    }
}

void quadtreeCompression(unsigned char* image, unsigned char* compressed, int x, int y, int blockWidth, int blockHeight, int width, int channels, double threshold, int minSize, int depth,int method) {
    if (depth > maxDepth) {
        maxDepth = depth;
    }
    nodeCount++;
    
    // jika image <min size langsung normColor
    if (blockWidth <= minSize || blockHeight <= minSize) {
        normColor(compressed, x, y, blockWidth, blockHeight, width, channels, image);
        return;
    }


    //cek variansi image
    double err = errMeasure(image, x, y, blockWidth, blockHeight, width, channels,method);
    // double variance = computeVariance(image, x, y, blockWidth, blockHeight, width, channels);

    //jika variansi < ambang batas varinasi (threshold)
    if (err < threshold) {
        normColor(compressed, x, y, blockWidth, blockHeight, width, channels, image);
        return;
    }

    int halfWidth = blockWidth / 2;
    int halfHeight = blockHeight / 2;

    quadtreeCompression(image, compressed, x, y, halfWidth, halfHeight, width, channels, threshold, minSize,depth+1,method);
    quadtreeCompression(image, compressed, x + halfWidth, y, blockWidth - halfWidth, halfHeight, width, channels, threshold, minSize,depth+1,method);
    quadtreeCompression(image, compressed, x, y + halfHeight, halfWidth, blockHeight - halfHeight, width, channels, threshold, minSize,depth+1,method);
    quadtreeCompression(image, compressed, x + halfWidth, y + halfHeight, blockWidth - halfWidth, blockHeight - halfHeight, width, channels, threshold, minSize,depth+1,method);
}

long getFileSize(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1;
}

void toLowerCase(char* str) {
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}
bool cekInputFormat(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext) return false;

    char lowerExt[10];
    strncpy(lowerExt, ext, sizeof(lowerExt));
    lowerExt[sizeof(lowerExt) - 1] = '\0';
    toLowerCase(lowerExt);

    return strcmp(lowerExt, ".jpg") == 0 ||
           strcmp(lowerExt, ".jpeg") == 0 ||
           strcmp(lowerExt, ".png") == 0;
}

bool cekOutputFormat(const char* str, const char* suffix) {
    if (!str || !suffix) return false;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr) return false;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
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





int main() {
    char imagePath[256];
    bool cekValidInput;
    printf("Masukkan path gambar (.jpg/.jpeg/.png): ");

    fgets(imagePath, sizeof(imagePath), stdin);
    imagePath[strcspn(imagePath, "\n")] = '\0';  // hapus newline

    // scanf("%s", imagePath);
    if (!cekInputFormat(imagePath)) {
        printf("Ekstensi file input tidak valid. Harus .jpg, .jpeg, atau .png\n");
        return -1;
    }

    int method;
    printf("Metode yang dapat digunakan: \n");
    printf("1. Variance \n");
    printf("2. Mean Absolute Deviation\n");
    printf("3. Max Pixel Difference\n");
    printf("4. Entropy\n");
    printf("Pilih metode yang ingin digunakan: ");
    scanf("%d", &method);

    double threshold;
    printf("Masukkan nilai threshold variansi: ");
    scanf("%lf", &threshold);

    int minSize;
    printf("Masukkan ukuran minimum blok: ");
    scanf("%d", &minSize);

    double targetCompress;
    printf("Masukkan target kompresi: ");
    scanf("%lf",&targetCompress);

    while (getchar() != '\n'); // buang newline

    char outputPath[256];
    printf("Masukkan path gambar hasil kompresi: ");
    // scanf("%s", outputPath);
    fgets(outputPath, sizeof(outputPath), stdin);
    outputPath[strcspn(outputPath, "\n")] = '\0';  // hapus newline
    if (!cekInputFormat(outputPath)) {
        printf("Ekstensi file output tidak valid. Harus .jpg, .jpeg, atau .png\n");
        return -1;
    }

    printf("\n");
    //==================================Proses=======================================
    clock_t start, end;
    start =clock();


    int width, height, channels;
    unsigned char *image = stbi_load(imagePath, &width, &height, &channels, 0);

    if (!image) {
        printf("Gagal membaca gambar!\n");
        return -1;
    }

    unsigned char *compressed = (unsigned char *)malloc(width * height * channels);
    memcpy(compressed, image, width * height * channels);


    long oriSize = getFileSize(imagePath);

    if (targetCompress > 0.0) {
        double low = 0.0, high = 1000.0;
        double thresholdNow = 0.0;
        double step = 50.0;
        double mid;
        unsigned char *temp = (unsigned char *)malloc(width * height * channels);
        long tempSize;
        char tempPath[20];
        const double tolerance = 0.005;
        int maxAttempt = 20;
        int attempt=0;

        double achieved=0.0;
        while (attempt <maxAttempt){

        
            mid = (low + high) / 2.0;
            memcpy(temp, image, width * height * channels);
            nodeCount = 0; maxDepth = 0;


            quadtreeCompression(image, temp, 0, 0, width, height, width, channels, mid, minSize,1,method);
            
           // stbi_write_jpg("temp.jpg", width, height, channels, temp, width * channels);
            if (cekOutputFormat(outputPath, ".jpg") || cekOutputFormat(outputPath, ".jpeg")) {
                strcpy(tempPath, "temp.jpg");
                stbi_write_jpg(tempPath, width, height, channels, temp, 60);
            } else if (cekOutputFormat(outputPath, ".png")) {
                strcpy(tempPath, "temp.png");
                stbi_write_png(tempPath, width, height, channels, temp, width * channels);
            } else {
                printf("Format output tidak dikenali.\n");
                free(temp);
                return -1;
            }
            
            tempSize = getFileSize(tempPath);
            achieved = 1.0 - ((double)tempSize / oriSize);

            // printf("Percobaan %2d: threshold = %8.3f , compress = %.4f , target = %.4f\n", attempt + 1, thresholdNow, achieved, targetCompress);

            if (fabs(achieved - targetCompress) < tolerance) {
                break;
            }

            if (thresholdNow == 0.0) {
                thresholdNow = step;  
            } else if (achieved < targetCompress) {
                thresholdNow += step;
            } else {
                break; //lanjut ke refinement
            }
            

            attempt++;        
        }
        
        step /= 2.0;
        while (attempt < maxAttempt) {
            memcpy(temp, image, width * height * channels);
            nodeCount = 0; maxDepth = 0;

            quadtreeCompression(image, temp, 0, 0, width, height, width, channels, thresholdNow, minSize, 1, method);

            if (cekOutputFormat(outputPath, ".jpg") || cekOutputFormat(outputPath, ".jpeg")) {
                strcpy(tempPath, "temp.jpg");
                stbi_write_jpg(tempPath, width, height, channels, temp, 60);
            } else {
                strcpy(tempPath, "temp.png");
                stbi_write_png(tempPath, width, height, channels, temp, width * channels);
            }

            tempSize = getFileSize(tempPath);
            achieved = 1.0 - ((double)tempSize / oriSize);

            // printf("Refine %2d: threshold = %8.3f ,  compress = %.4f , target = %.4f\n", attempt + 1, thresholdNow, achieved, targetCompress);

            if (fabs(achieved - targetCompress) < tolerance) {
                break;
            }

            if (achieved < targetCompress) {
                thresholdNow += step;
            } else {
                thresholdNow = fmax(0.0, thresholdNow - step);
            }

            step /= 2.0;
            attempt++;
        }

        threshold = thresholdNow;


        free(temp);
        remove(tempPath);
    }

    nodeCount = 0; maxDepth = 0;
    quadtreeCompression(image, compressed, 0, 0, width, height, width, channels, threshold, minSize,1,method);

    //============================output ================================



    if (cekOutputFormat(outputPath, ".jpg") || cekOutputFormat(outputPath, ".jpeg")) {
        stbi_write_jpg(outputPath, width, height, channels, compressed, 60); // 60 = kualitas
    } else if (cekOutputFormat(outputPath, ".png")) {
        stbi_write_png(outputPath, width, height, channels, compressed, width * channels);
    } else {
        printf("Format output tidak dikenali. Harus .jpg/.jpeg/.png\n");
        return -1;
    }
    
    printf("Gambar berhasil dikompres ('%s')\n",outputPath);

    end = clock();
    double executeTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Waktu eksekusi: %.3f second\n", executeTime);

    long compressSize;
    compressSize = getFileSize(outputPath);

    if (oriSize > 0 && compressSize > 0) {
        double compressionRatio = 1.0 - ((double)compressSize / oriSize);
        printf("Ukuran gambar sebelum: %ld byte\n", oriSize);
        printf("Ukuran gambar setelah: %ld byte\n", compressSize);
        printf("Persentase Kompresi: %.2f%%%%\n", compressionRatio * 100.0);
    }

    printf("Kedalaman pohon: %d\n", maxDepth);
    printf("Banyak simpul: %d\n", nodeCount);

    //hitung ssim gambar sebelum dan setlah kompres
    // double ssim = hitungSSIM(image, compressed, width, height);
    // printf("Nilai SSIM: %lf\n", ssim);
    
    //===========

    stbi_image_free(image);
    free(compressed);
    return 0;
}

