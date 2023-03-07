#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <iostream>
#include "bmp_reader.cuh"
using namespace std;

#ifndef BMP_READER_C
#define BMP_READER_C
const int NT = 7;
int N = 1000;
int count_cuda[7];

BMP_File* loadBMP(const char* fname)
{
	FILE* fp = fopen(fname, "r");
	if (!fp)
	{
		cout << "Error!";
		exit(0);
	}

	BMP_File* bmp = (BMP_File*)malloc(sizeof(BMP_File));
	fread(&bmp->bh, sizeof(BMP_Header), 1, fp);
	fread(&bmp->dh, sizeof(DIB_Header), 1, fp);
	bmp->data = (unsigned char*)malloc(bmp->dh.data_size);
	fseek(fp, bmp->bh.pixel_offset, SEEK_SET);
	fread(bmp->data, bmp->dh.data_size, 1, fp);
	fclose(fp);
	return bmp;
}

void freeBMP(BMP_File* bmp)
{
	if (bmp)
	{
		free(bmp);
	}
}

#endif