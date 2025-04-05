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

void quadtreeCompression(unsigned char* image, unsigned char* compressed, int x, int y, int blockWidth, int blockHeight, int width, int channels, double threshold, int minSize, int depth) {
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
    double variance = computeVariance(image, x, y, blockWidth, blockHeight, width, channels);

    //jika variansi < ambang batas varinasi (threshold)
    if (variance < threshold) {
        normColor(compressed, x, y, blockWidth, blockHeight, width, channels, image);
        return;
    }

    int halfWidth = blockWidth / 2;
    int halfHeight = blockHeight / 2;

    quadtreeCompression(image, compressed, x, y, halfWidth, halfHeight, width, channels, threshold, minSize,depth+1);
    quadtreeCompression(image, compressed, x + halfWidth, y, blockWidth - halfWidth, halfHeight, width, channels, threshold, minSize,depth+1);
    quadtreeCompression(image, compressed, x, y + halfHeight, halfWidth, blockHeight - halfHeight, width, channels, threshold, minSize,depth+1);
    quadtreeCompression(image, compressed, x + halfWidth, y + halfHeight, blockWidth - halfWidth, blockHeight - halfHeight, width, channels, threshold, minSize,depth+1);
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
        double mid;
        unsigned char *temp = (unsigned char *)malloc(width * height * channels);
        long tempSize;
        char tempPath[20];

        for (int i = 0; i < 15; i++) {
            mid = (low + high) / 2.0;
            memcpy(temp, image, width * height * channels);
            nodeCount = 0; maxDepth = 0;


            quadtreeCompression(image, temp, 0, 0, width, height, width, channels, mid, minSize,1);
            
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
            double achieved = 1.0 - ((double)tempSize / oriSize);

            if (achieved < targetCompress)
                low = mid;
            else
                high = mid;
        }
        threshold = mid;
        free(temp);
        remove(tempPath);
    }

    nodeCount = 0; maxDepth = 0;
    quadtreeCompression(image, compressed, 0, 0, width, height, width, channels, threshold, minSize,1);

    //============================output ================================



    if (cekOutputFormat(outputPath, ".jpg") || cekOutputFormat(outputPath, ".jpeg")) {
        stbi_write_jpg(outputPath, width, height, channels, compressed, 60); // 60 = kualitas
    } else if (cekOutputFormat(outputPath, ".png")) {
        stbi_write_png(outputPath, width, height, channels, compressed, width * channels);
    } else {
        printf("Format output tidak dikenali. Harus .jpg/.jpeg/.png\n");
        return -1;
    }
    
    printf("Gambar telah terkompresi sebagai '%s'\n",outputPath);

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

    //===========

    stbi_image_free(image);
    free(compressed);
    return 0;
}

