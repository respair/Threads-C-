#pragma once
#ifndef BMP_READER_H
#define BMP_READER_H

#pragma pack(1)
typedef struct BMP_Header
{
	unsigned char ID[2];
	unsigned int file_size;
	unsigned char unused[4];
	unsigned int pixel_offset;
} BMP_Header;

typedef struct DIB_Header
{
	unsigned int header_size;
	unsigned int width;
	unsigned int height;
	unsigned short color_planes;
	unsigned short bytes_per_pixel;
	unsigned int comp;
	unsigned int data_size;
	unsigned int pwidth;
	unsigned int pheight;
	unsigned int colors_count;
	unsigned int imp_colors_count;
} DIB_Header;

typedef struct BMP_File
{
	BMP_Header bh;
	DIB_Header dh;
	unsigned char* data;
} BMP_File;
#pragma pop

BMP_File* loadBMP(const char* fname);
void freeBMP(BMP_File* bmp);

#endif