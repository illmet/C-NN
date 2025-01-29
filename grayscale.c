#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Basic pixel structure for RGB
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} Pixel;

// Image structure
typedef struct {
    int width;
    int height;
    Pixel* pixels;
    uint8_t* grayscale;  // For storing grayscale version
} Image;

// BMP header structures
#pragma pack(push, 1)  // Ensure struct packing
typedef struct {
    uint16_t signature;      // "BM"
    uint32_t filesize;      // Size of the BMP file in bytes
    uint16_t reserved1;     // Reserved
    uint16_t reserved2;     // Reserved
    uint32_t dataOffset;    // Offset to image data in bytes
} BMPHeader;

typedef struct {
    uint32_t headerSize;     // Header size in bytes
    int32_t width;          // Width of the image
    int32_t height;         // Height of the image
    uint16_t planes;        // Number of color planes
    uint16_t bitsPerPixel;  // Bits per pixel
    uint32_t compression;   // Compression type
    uint32_t imageSize;     // Image size in bytes
    int32_t xPixelsPerM;    // Pixels per meter in x axis
    int32_t yPixelsPerM;    // Pixels per meter in y axis
    uint32_t colorsUsed;    // Number of colors used
    uint32_t colorsImp;     // Important colors
} BMPInfoHeader;
#pragma pack(pop)

// Function to read a BMP file
Image* readBMP(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file\n");
        return NULL;
    }

    BMPHeader header;
    BMPInfoHeader infoHeader;

    // Read headers
    fread(&header, sizeof(BMPHeader), 1, file);
    fread(&infoHeader, sizeof(BMPInfoHeader), 1, file);

    // Verify that it's a BMP file
    if (header.signature != 0x4D42) {  // "BM" in hex
        printf("Not a BMP file\n");
        fclose(file);
        return NULL;
    }

    // Allocate memory for image
    Image* img = (Image*)malloc(sizeof(Image));
    img->width = infoHeader.width;
    img->height = infoHeader.height;
    img->pixels = (Pixel*)malloc(img->width * img->height * sizeof(Pixel));
    img->grayscale = NULL;

    // Calculate row padding
    int paddingSize = (4 - (img->width * sizeof(Pixel)) % 4) % 4;

    // Read pixel data
    for (int y = 0; y < img->height; y++) {
        // Read row of pixels
        fread(&img->pixels[y * img->width], sizeof(Pixel), img->width, file);
        // Skip padding
        fseek(file, paddingSize, SEEK_CUR);
    }

    fclose(file);
    return img;
}

// Convert to grayscale using luminosity method
// Weights based on human perception: R:0.299, G:0.587, B:0.114
void convertToGrayscale(Image* img) {
    if (!img || !img->pixels) return;

    // Allocate memory for grayscale data if not already allocated
    if (!img->grayscale) {
        img->grayscale = (uint8_t*)malloc(img->width * img->height);
    }

    for (int i = 0; i < img->width * img->height; i++) {
        img->grayscale[i] = (uint8_t)(
            0.299f * img->pixels[i].red +
            0.587f * img->pixels[i].green +
            0.114f * img->pixels[i].blue
        );
    }
}

// Save grayscale image as BMP
void saveGrayscaleBMP(const char* filename, Image* img) {
    if (!img || !img->grayscale) return;

    FILE* file = fopen(filename, "wb");
    if (!file) return;

    // Calculate file size and padding
    int paddingSize = (4 - (img->width % 4)) % 4;
    int dataSize = img->width * img->height + (paddingSize * img->height);
    int fileSize = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + dataSize;

    // Prepare headers
    BMPHeader header = {
        .signature = 0x4D42,  // "BM"
        .filesize = fileSize,
        .reserved1 = 0,
        .reserved2 = 0,
        .dataOffset = sizeof(BMPHeader) + sizeof(BMPInfoHeader)
    };

    BMPInfoHeader infoHeader = {
        .headerSize = sizeof(BMPInfoHeader),
        .width = img->width,
        .height = img->height,
        .planes = 1,
        .bitsPerPixel = 8,  // 8 bits for grayscale
        .compression = 0,
        .imageSize = dataSize,
        .xPixelsPerM = 0,
        .yPixelsPerM = 0,
        .colorsUsed = 256,  // Grayscale color table
        .colorsImp = 256
    };

    // Write headers
    fwrite(&header, sizeof(BMPHeader), 1, file);
    fwrite(&infoHeader, sizeof(BMPInfoHeader), 1, file);

    // Write grayscale color table
    for (int i = 0; i < 256; i++) {
        uint8_t gray = i;
        fwrite(&gray, 1, 1, file);  // Blue
        fwrite(&gray, 1, 1, file);  // Green
        fwrite(&gray, 1, 1, file);  // Red
        fwrite(&gray, 1, 1, file);  // Reserved
    }

    // Write pixel data with padding
    uint8_t padding[3] = {0, 0, 0};
    for (int y = 0; y < img->height; y++) {
        fwrite(&img->grayscale[y * img->width], 1, img->width, file);
        fwrite(padding, 1, paddingSize, file);
    }

    fclose(file);
}

// Free image memory
void freeImage(Image* img) {
    if (img) {
        free(img->pixels);
        free(img->grayscale);
        free(img);
    }
}

#endif // IMAGE_PROCESSOR_H
