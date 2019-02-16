#include <stdlib.h>
#include <stdio.h>
#include <regex>
#include "PixelArray.h"

PixelArray::PixelArray(char* inFilename, char flipVertical) {
    std::string inFileString(inFilename);
    std::regex tgaFile(".*\\.tga");
    pixels = NULL;
    if (!regex_match(inFileString, tgaFile)) {
        fprintf(stderr, "File %s isn't a supported format.\n", inFilename);
        return;
    }
    FILE* f = fopen(inFilename, "r");
    if (f == NULL) {
        fprintf(stderr, "Could not open file %s\n", inFilename);
        return;
    }
    char type;
    char depth;
    char lower, upper;
    fseek(f, 2, SEEK_CUR);
    type = fgetc(f);
    if (type != 2) {
        fprintf(stderr, "Tga is not uncompressed RGB. Returning.\n");
        return;
    }
    fseek(f, 5, SEEK_CUR);
    
    lower = fgetc(f);
    upper = fgetc(f);
    originX = (upper << 8) + lower;
    
    lower = fgetc(f);
    upper = fgetc(f);
    originY = (upper << 8) + lower;
    
    lower = fgetc(f);
    upper = fgetc(f);
    width = (upper << 8) + lower;
    lower = fgetc(f);
    upper = fgetc(f);
    height = (upper << 8) + lower;
    //fprintf(stderr, "Image to be read is %d by %d\n", width, height);
    fprintf(stderr, "Origin is at %d, %d\n", originX, originY);
    depth = fgetc(f);
    if (depth != 24) {
        fprintf(stderr, "Image has %d bits per pixel, which is unsupported.\n", depth);
        return;
    }
    fseek(f, 1, SEEK_CUR);
    pixels = new unsigned char[width*height*4];
    int idx = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            idx = 4*(i*width + j);
            if (flipVertical) {
                idx = 4*((height-i-1)*width + j);
            }
            pixels[idx + 3] = 255;
            pixels[idx + 2] = fgetc(f);
            pixels[idx + 1] = fgetc(f);
            pixels[idx] = fgetc(f);
        }
    }
    fclose(f);
    filename = new char[strlen(inFilename)];
    strcpy(filename, inFilename);
}

PixelArray::~PixelArray() {
    delete pixels;
    pixels = NULL;
}

int PixelArray::saveImage(char* filename, char flipVertical) {
    if (pixels == NULL) {
        fprintf(stderr, "Warning: attempted to save empty image. Exiting.");
        return -1;
    }
    return saveImage(filename, pixels, width, height, originX, originY, flipVertical);
}

int PixelArray::saveImage(char* filename, unsigned char* RGBA, int inWidth, int inHeight, int inOriginX, int inOriginY, char flipVertical) {
    FILE* f = fopen(filename, "w+");
    if (f == NULL) {
        return -1;
    }
	char header[8] = {
		0,
		0,
		2,
		0, 0,
		0, 0,
		0
	};
    
	for (int i = 0; i < 8; i++) {
		putc(header[i], f);
	}
    putc((inOriginX & 0x00FF), f);
	putc((inOriginX & 0xFF00) >> 8, f);
	putc((inOriginY & 0x00FF), f);
	putc((inOriginY & 0xFF00) >> 8, f);
	putc((inWidth & 0x00FF), f);
	putc((inWidth & 0xFF00) >> 8, f);
	putc((inHeight & 0x00FF), f);
	putc((inHeight & 0xFF00) >> 8, f);
	putc(24, f);
	putc(0, f);
    int idx;
    for (int i = 0; i< inHeight; i++) {
        for (int j = 0; j < inWidth; j++) {
            idx = 4*(i*inWidth + j);
            if (flipVertical) {
                idx = 4*((inHeight-i-1)*inWidth + j);
            }
            putc(RGBA[idx + 2], f);
            putc(RGBA[idx + 1], f);
            putc(RGBA[idx + 0], f);
        }
    }
	fclose(f);
    return 0;
}