#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include<time.h>
#include <stdbool.h>
#include <ctype.h>

#include "quadtreeCompression.h"
#include "errMethod.h"
#include "inputOutput.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


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

