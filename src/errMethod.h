#ifndef ERR_METHOD_H
#define ERR_METHOD_H

double computeVariance(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels);
double computeMAD(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels);
double computeMPD(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels);
double computeEntropy(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels);
double errMeasure(unsigned char* image, int x, int y, int blockWidth, int blockHeight, int width, int channels, int method);

#endif
