#pragma once
#include <fstream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <functional>

class bmp {
    private:
        std::string imgPath;
        int height, width, hOf, wOf;
        bool painted;

        #pragma pack(push, 1)
        struct BMPHeader {
            uint16_t bfType;
            uint32_t bfSize;
            uint16_t bfReserved1;
            uint16_t bfReserved2;
            uint32_t bfOffBits;
        };

        struct DIBHeader {
            uint32_t biSize;
            int32_t biWidth;
            int32_t biHeight;
            uint16_t biPlanes;
            uint16_t biBitCount;
            uint32_t biCompression;
            uint32_t biSizeImage;
            int32_t biXPelsPerMeter;
            int32_t biYPelsPerMeter;
            uint32_t biClrUsed;
            uint32_t biClrImportant;
        };

        #pragma pack(pop)
        struct RGB {
            uint8_t blue;
            uint8_t green;
            uint8_t red;
        };

        struct check {
            size_t operator()(const point2& p) const {
                return (std::hash<int>()(p.x_i)) ^ (std::hash<int>()(p.y_i));
            }
        };

        std::unordered_map<point2, RGB, check> pixels;

    public:
        //Normal init.
        bmp(std::string& imgPath, int width, int height) : imgPath(imgPath), width(width), height(height), wOf(0), hOf(0), painted(false) {}
        //Fully init.
        bmp(std::string& imgPath, int width, int height, int widthOffset, int heightOffset) : imgPath(imgPath), width(width), height(height), wOf(widthOffset), hOf(heightOffset), painted(false) {}

        void createPixels(int &status) {
            std::ifstream bmp(this->imgPath, std::ios::binary);
            if (!bmp) {
                status = 1;
                return;
            }

            BMPHeader bmpHeader;
            bmp.read(reinterpret_cast<char*>(&bmpHeader), sizeof(BMPHeader));
            if (bmpHeader.bfType != 0x4D42) {//Check if file really is a BMP.
                status = 2;
                return;
            }

            DIBHeader dibHeader;
            bmp.read(reinterpret_cast<char*>(&dibHeader), sizeof(DIBHeader));
            if (dibHeader.biBitCount != 24) {
                status = 3;
                return;
            }

            int32_t width = dibHeader.biWidth;
            int32_t height = dibHeader.biHeight;
            for (int x = 0; x < this->width; x++) {
                for (int y = 0; y < this->height; y++) {
                    int32_t adjustedY = height - y - 1;//Inverted y-dim. (y=0 was on the bottom of the texture file, now it's the top.)
                    int rowSize = (width * 3 + 3) & (~3);
                    int pixelOffset = bmpHeader.bfOffBits + adjustedY * rowSize + x * 3;
                    bmp.seekg(pixelOffset, std::ios::beg);
                    RGB pixel_;
                    bmp.read(reinterpret_cast<char*>(&pixel_), sizeof(RGB));
                    point2 pos{ x, y };
                    pixels[pos] = pixel_;
                }
            }
            status = 0;
        }

        //Getter and setter for BMP
        void setBMP(std::string imgPath) { this->imgPath = imgPath; }
        std::string getBMP() { return this->imgPath; }
        //

        //Getters for width, height and offsets
        int getWidth() { return this->width; }
        int getHeight() { return this->height; }
        int getWOf() { return this->wOf; }
        int getHOf() { return this->hOf; }
        //

        //Painted getter and setter
        bool isPainted() { return this->painted; }
        void setPainted(bool stat) { this->painted = stat; }
        //

        std::unordered_map<point2, RGB, check> getPixels() { return this->pixels; }

        point3 getPixelValue(int x, int y) {//Get RGB value from pixel coordinates (Based on BMP format)
            point2 pos{ x, y };
            return point3{ pixels[pos].red, pixels[pos].green, pixels[pos].blue };
        }
};
