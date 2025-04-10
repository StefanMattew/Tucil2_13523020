#ifndef QUADTREE_COMPRESSION_H
#define QUADTREE_COMPRESSION_H

extern int maxDepth;
extern int nodeCount;

void quadtreeCompression(unsigned char* image, unsigned char* compressed, int x, int y, int blockWidth, int blockHeight, int width, int channels, double threshold, int minSize, int depth, int method);

#endif
