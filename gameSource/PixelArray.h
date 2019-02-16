#ifndef PIXARRAY
#define PIXARRAY
class PixelArray {
    public:
        PixelArray(char* filename, char flipVertical);
        ~PixelArray();
        int width;
        int height;
        int originX;
        int originY;
        char* filename;
        unsigned char* pixels;
        int saveImage(char* filename, char flipVertical);
        static int saveImage(char* filename, unsigned char* RGBA, int inWidth, int inHeight, int inOriginX, int inOriginY, char flipVertical);
    
};
#endif