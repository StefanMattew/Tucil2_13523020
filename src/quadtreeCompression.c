#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include<time.h>
#include <stdbool.h>
#include <ctype.h>

#include "errMethod.h"


int maxDepth = 0;
int nodeCount = 0;

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